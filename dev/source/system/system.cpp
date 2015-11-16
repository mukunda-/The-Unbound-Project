//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2015, Mukunda Johnson, All rights reserved. =========//

#include "stdafx.h"
#include "system.h"
#include "module.h"
#include "console.h"
#include "commands.h"
#include "console/console.h"

#include "util/fopen2.h"
#include "util/codetimer.h"

#include "events/eventinterface.h"
#include "events/event.h"

#pragma warning( disable : 4996 )

//-----------------------------------------------------------------------------
namespace System {

Main *g_main = nullptr;

//-----------------------------------------------------------------------------
namespace {
	
	//-------------------------------------------------------------------------
	void Command_Quit( Util::ArgString &args ) {
		
		System::Shutdown( "Quit command." );
	} 
}

//-----------------------------------------------------------------------------
Main::Main( int threads, StartMode start_mode ) 
		: m_strand( m_service() ), m_events( new EventInterface() ) {

	assert( g_main == nullptr );

	g_main = this;

	m_start_mode = start_mode;
	m_live       = true;
	m_started    = false;

	m_lua_state = luaL_newstate();

	if( start_mode == StartMode::DEDICATED_MAIN ) {

		m_using_strand = false;

	} else if( start_mode == StartMode::JOIN_WORK ) {

		m_using_strand = true;

	} else if( start_mode == StartMode::PASS ) {

		m_using_strand = true;

	}

	m_console.reset( new ::Console::Instance() );

	m_service.Run( threads );
	 
	AddGlobalCommand( "quit", "Quit program.", Command_Quit );
}

//-----------------------------------------------------------------------------
Main::~Main() {
	Shutdown( "System Destructed." );

	{
		// block until shutdown completes; normally the shutdown would
		// be complete already, but in some cases (eg unit testing) the
		// system goes out of scope before the program is finished.

		std::unique_lock<std::recursive_mutex> lock( m_mutex );
		m_cvar_shutdown.wait( lock, 
			[&]() { return m_shutdown_complete; } 
		);
	}

	// we clear this here because the command instance destructors
	// need access to Main.
	m_global_commands.clear();

	g_main = nullptr;
}

//-----------------------------------------------------------------------------
void Main::Post( std::function<void()> handler, bool main_thread, int delay ) {
	
	if( main_thread ) {
		
		if( m_using_strand ) {
			// TODO delay
			m_strand.post( handler );
		} else {
			m_service_main.Post( handler, delay );
		}
		
	} else {
		m_service.Post( handler, delay );
	}
}

//-----------------------------------------------------------------------------
void Main::RegisterModule( Module *module_ptr ) {

	std::unique_ptr<Module> module( module_ptr );
	// must be called in the system strand
	
	if( m_module_map.count( module->GetName() ) ) {
		throw std::runtime_error( "Duplicate module name." );
	}

	m_module_map[ module->GetName() ] = module.get();
	
	auto level = module->GetLevel();

	auto iterator = m_modules.begin();

	// find and seek to the end of this module's bracket.
	for( ; iterator != m_modules.end(); iterator++ ) {

		if( (*iterator)->GetLevel() > level ) {
			break;
		}
	}
	
	m_modules.insert( iterator, std::move( module ));

	module_ptr->OnLoad();
}

//-----------------------------------------------------------------------------
void Main::Start() { 
	m_started = true;

	Post( [&]() {
		// prepare modules
		for( auto &i : m_modules ) {
			i->OnPrepare();
		}

		// start modules
		for( auto &i : m_modules ) {
			i->OnStart();
		}

		m_clock.Reset();

		if( m_start_mode != StartMode::PASS ) {
			// we dont do frame updates in PASS mode
			// PASS is used for unit testing.

			Post( std::bind(&Main::OnFrame, this) );
		}

	}, true, 0 );

	if( m_start_mode == StartMode::DEDICATED_MAIN ) {
		m_service_main.Join();
	} else if( m_start_mode == StartMode::JOIN_WORK ) {
		m_service.Join();
	} else if( m_start_mode == StartMode::PASS ) {
		return;
	}
}

//-----------------------------------------------------------------------------
void Main::OnFrame() {

	m_last_time = m_time;
	m_time = m_clock.GetTime();
	
	for( auto &i : m_modules ) {
		i->OnFrame();
	}

	if( m_live ) {
		Post( std::bind( &Main::OnFrame, this ), true, 0 );
	}
}
  
//-----------------------------------------------------------------------------
void Main::Shutdown( const Stref &reason ) {
	std::string reason_copy = reason;

	// execute in main strand 
	Post( [this, reason_copy]() {
		if( !m_live ) return; // already shut down.
	
		::Console::Print( "Shutting down. (%s)", reason_copy );

		// we lock here so async access is paused during the
		// shutdown preparation
		//
		std::lock_guard<std::recursive_mutex> lock(m_mutex);
		m_live = false;

		for( auto i = m_modules.rbegin(); i != m_modules.rend(); i++ ) {
			(*i)->OnShutdown();
		} 

		if( m_busy_modules == 0 && !m_system_end_posted ) {
			// all modules are idle.
			m_system_end_posted = true;

			Post( std::bind( &Main::SystemEnd, this ), true, 0 );
		}
	});
}

//-----------------------------------------------------------------------------
void Main::OnModuleIdle( Module &module ) {
	std::lock_guard<std::recursive_mutex> lock( m_mutex );

	m_busy_modules--;

	if( !m_live && m_busy_modules == 0 && !m_system_end_posted ) {
		// shutdown is in progress, and all modules are idle 
		m_system_end_posted = true;
		
		Post( std::bind( &Main::SystemEnd, this ), true, 0 );
	}
}

//-----------------------------------------------------------------------------
void Main::OnModuleBusy( Module &module ) {
	std::lock_guard<std::recursive_mutex> lock( m_mutex ); 
	m_busy_modules++;
}

//-----------------------------------------------------------------------------
bool Main::AnyModulesBusy() {
	for( auto &i : m_modules ) {
		if( i->Busy() ) {
			return true;
		}
	}
	return false;
}

//-----------------------------------------------------------------------------
void Main::SystemEnd() {
	if( AnyModulesBusy() ) {
		// there is a logical error somewhere, no modules should
		// be busy at this point.
		 
		assert( !"Modules busy during system end." );
		return; 
	}

	// this function should only run once.
	assert( !m_shutdown_complete );

	// delete all modules
	for( auto i = m_modules.rbegin(); i != m_modules.rend(); i++ ) {
		(*i)->OnUnload();
		(*i).reset();
	}

	{
		std::lock_guard<std::recursive_mutex> lock( m_mutex );
		m_shutdown_complete = true;
	}
	
	m_cvar_shutdown.notify_all();

	m_service.Finish( false );
	m_service_main.Finish( false );
}

//-----------------------------------------------------------------------------
void ExecuteCommand( const Stref &command_string, bool command_only ) {
	g_main->ExecuteCommand( command_string, command_only );
}
 
//-----------------------------------------------------------------------------
void Main::ExecuteCommand( const Stref &command_string, 
						   bool command_only ) {

	// copy command
	char command[1024];
	Util::CopyString( command, *command_string );
	Util::TrimString( command );
	{
		// strip comment
		char *comment = strstr( command, "//" );
		if( comment ) comment[0] = 0;
	}

	char name[64];
	const char *next = Util::BreakString( command, name );
	if( name[0] == 0 ) {
		::Console::Print( "" );
		return;
	}

	::Console::Print( "\n>>> %s", command_string );
	
	if( TryExecuteCommand( command_string ) ) {
		return;
	}
	 
	System::Variable *var = System::Variables::Find( name );
	if( !var || command_only ) {

		::Console::Print( "Unknown command: \"%s\"", name );
		return;
	} 

	char value[512];
	Util::CopyString( value, next );
	Util::TrimString( value );
	Util::StripQuotes( value );
	
	if( Util::StrEmpty( value ) ) {
		var->PrintInfo();
	} else {
		var->SetString( value );
	}
}

//-----------------------------------------------------------------------------
bool ExecuteScript( const Stref &file ) {
	return g_main->ExecuteScript( file );
}

//-----------------------------------------------------------------------------
bool Main::ExecuteScript( const Stref &file ) {
	FILE *f = fopen2( *file, "r" );
	if( !f ) {
		::Console::Print( "Script not found: \"%s\"", *file );
		return false;
	}

	char line[1024];
	::Console::Print( "Executing script: \"%s\"", *file );

	Util::CodeTimer timer;

	while( !feof(f) ) {
		fgets( line, sizeof line, f );
		ExecuteCommand( line );
	}

	::Console::Print( "Finished executing script: \"%s\", time=%s", file, 
		Util::RoundDecimal( timer.Duration(), 2 ));
		        
	return true;
	
}

//----------------------------------------------------------------------------
void Main::Log( const Stref &message ) {
	// todo: log to file
	::Console::Print( message ); 
}

//-----------------------------------------------------------------------------
void Main::LogError( const Stref &message ) {
	::Console::PrintErr( message );
	// todo: log to error file
}

//-----------------------------------------------------------------------------
Service     &GetService()                                      { return g_main->GetService();          }
void         Finish()                                          { GetService().Finish( true );          }
void         RegisterModule( Module *m )                       { g_main->RegisterModule( m );          }
void         RegisterModule( ModulePtr &&m )                   { RegisterModule( m.release() );        }
void         Start()                                           { g_main->Start();                      }
void         Shutdown( const Stref &r )                        { g_main->Shutdown( r );                }
void         Log( const Stref &m )                             { g_main->Log( m );                     }
void         LogError( const Stref &m )                        { g_main->LogError( m );                } 
bool         Live()                                            { return g_main->Live();                }
void         Post( std::function<void()> h, bool m, int d )    { g_main->Post( h, m, d );              }

void         RegisterEvent( const Event::Info &i )             { g_main->Events().Register( i );       }
EventHookPtr HookEvent( const Event::Info &i, EventHandler h ) { return g_main->Events().Hook( i, h ); }
void         SendEvent( Event &e )                             { g_main->Events().Send( e );           }
double       Time()                                            { return g_main->GetTime();             }
double       LastTime()                                        { return g_main->GetLastTime();         }

/*void Join() {g_main->GetService().Join();}*/
 

}

