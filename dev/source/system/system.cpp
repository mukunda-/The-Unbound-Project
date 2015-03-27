//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2015, Mukunda Johnson, All rights reserved. =========//

#include "stdafx.h"
#include "system/system.h"
#include "system/module.h"
#include "system/console.h"
#include "system/commands.h"
#include "console/console.h"
#include "util/fopen2.h"
#include "util/codetimer.h"

#pragma warning( disable : 4996 )

//-----------------------------------------------------------------------------
namespace System {

Main *g_main;

//-----------------------------------------------------------------------------
namespace {
	
	//-------------------------------------------------------------------------
	void Command_Quit( Util::ArgString &args ) {
		
		System::Shutdown();
	}

}
  
//-----------------------------------------------------------------------------
Service &GetService() {
	assert(g_main);
	return g_main->GetService();
}

//-----------------------------------------------------------------------------
void Finish() {
	GetService().Finish( true );
}

//-----------------------------------------------------------------------------
void RegisterModule( std::unique_ptr<Module> &&module ) {

	Post( std::bind( &Main::RegisterModule, g_main, module.release() ));
}

//-----------------------------------------------------------------------------
void RegisterModule( Module *module ) {
	
	Post( std::bind( &Main::RegisterModule, g_main, module ));
}

//-----------------------------------------------------------------------------
void Shutdown() {
	::Console::Print( "Shutting down." );
	g_main->Shutdown();
}

//-----------------------------------------------------------------------------
void Log( const std::string &message ) {
	::Console::Print( message );
	// todo: log to file
} 

//-----------------------------------------------------------------------------
void LogError( const std::string &message ) {
	::Console::PrintErr( message );
	// todo: log to error file
}

//-----------------------------------------------------------------------------
bool Live() {
	return g_main->Live();
}

//-----------------------------------------------------------------------------
void Join() {
	g_main->GetService().Join();
}

//-----------------------------------------------------------------------------
void Post( std::function<void()> handler, bool main, int delay ) {
	g_main->PostSystem( handler, main, delay );
}

//-----------------------------------------------------------------------------
Main::Main( int threads ) : m_strand( m_service() ) {
	assert( g_main == nullptr );
	g_main = this;

	m_console.reset( new ::Console::Instance() );

	m_service.Run( threads );
	m_live = true;

	AddGlobalCommand( "quit", "Quit program.", Command_Quit );
}

//-----------------------------------------------------------------------------
Main::~Main() {
	Shutdown();

	{
		// block until shutdown completes
		std::unique_lock<std::mutex> lock( m_mutex );
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
void Main::PostSystem( std::function<void()> handler, 
						   bool main, int delay ) {
	
	if( main ) {
		m_service.Post( m_strand.wrap( handler ), delay );
	} else {
		m_service.Post( handler, delay );
	}
}

//-----------------------------------------------------------------------------
void Main::RegisterModule( Module *module_ptr ) {

	std::unique_ptr<Module> module(module_ptr);
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
}

//-----------------------------------------------------------------------------
void Main::Start() {

	for( auto &i : m_modules ) {
		i->OnStart();
	}

	Join();
}

//-----------------------------------------------------------------------------
Service &Main::GetService() {
	return m_service;
}

//-----------------------------------------------------------------------------
void Main::Shutdown() {
	m_strand.post( std::bind( &Main::ShutdownEx, this ));
}

//-----------------------------------------------------------------------------
void Main::ShutdownEx() {
	if( !m_live ) return; // already shut down.
	
	std::lock_guard<std::mutex> lock(m_mutex);
	m_live = false;

	for( auto i = m_modules.rbegin(); i != m_modules.rend(); i++ ) {
		(*i)->OnShutdown();
	} 

	if( m_busy_modules == 0 ) {
		// all modules are idle.
		m_strand.post( std::bind( &Main::SystemEnd, this ));
	}
}

//-----------------------------------------------------------------------------
void Main::OnModuleIdle( Module &module ) {
	std::lock_guard<std::mutex> lock(m_mutex);

	m_busy_modules--;

	if( !m_live && m_busy_modules == 0 ) {
		// shutdown is in progress, and all modules are idle 
		m_strand.post( std::bind( &Main::SystemEnd, this ));
	}
}

//-----------------------------------------------------------------------------
void Main::OnModuleBusy( Module &module ) {
	std::lock_guard<std::mutex> lock(m_mutex);
	
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
		std::lock_guard<std::mutex> lock( m_mutex );
		m_shutdown_complete = true;
	}

	m_cvar_shutdown.notify_all();
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

}

