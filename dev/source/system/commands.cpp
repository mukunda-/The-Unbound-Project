//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//

#include "stdafx.h"
#include "commands.h"
#include "util/trie.h"
#include "util/argstring.h"

//---------------------------------------------------------------------------------------
namespace System { namespace Console {
	
//---------------------------------------------------------------------------------------
namespace {
	

	Util::Trie< Instance* > g_command_trie;
	std::vector<Command::Pointer> g_global_commands;
}

//---------------------------------------------------------------------------------------
class Instance {

private:
	std::string m_name;
	std::string m_desc;
	std::vector<Command::Handler> m_handlers;

	typedef std::shared_ptr<Instance> Pointer;

	//---------------------------------------------------------------------------------------
	Instance( const char *name, const char *desc ) {
		// this class is not created directly
		// new instance, we push it into the trie,
		 
		if( desc != nullptr ) {
			m_desc = desc;
		}
		assert( name );
		m_name = name;

		// push the new instance into the trie
		// as soon as all references to this are deleted
		// the destructor will pull it from the trie.
		g_command_trie.Set( name, this );
	}

	void AddHandler(  Command::Handler handler, bool highprio ) {
		if( highprio ) {
			m_handlers.insert( m_handlers.begin(), handler );
		} else {
			m_handlers.push_back( handler );
		}
	}

public:	
	
	~Instance() {
		g_command_trie.Reset( m_name.c_str() );
	}

	//---------------------------------------------------------------------------------------
	static Pointer Create( const char *name, Command::Handler handler, const char *desc, bool highprio ) {
		Instance *inst;
		if( g_command_trie.Get(name, inst) ) {
			inst->AddHandler( handler, highprio );
			return Pointer(inst);
		}
		
		inst = new Instance( name, desc );
		inst->AddHandler( handler, highprio );
		return Pointer(inst);
	}

	//---------------------------------------------------------------------------------------
	void Remove( Command::Handler handler ) {
		for( auto i = m_handlers.begin(); i != m_handlers.end(); i++ ) {
			if( *i == handler ) {
				m_handlers.erase( i );
				return;
			}
		}
	}

	//---------------------------------------------------------------------------------------
	bool HasHandlers() {
		return m_handlers.size() != 0;
	}

	//---------------------------------------------------------------------------------------
	void Execute( const char *cmdstring ) {
		Util::ArgString args( cmdstring );

		for( auto handler : m_handlers ) {
		
			int result = handler(args);	
			if( result == COMMAND_STOP ) break;
		}
	}
};


//---------------------------------------------------------------------------------------
Command::Command( const char *name, Handler handler, const char *desc, bool high_priority ) {
	m_handler = handler;
	m_inst = Instance::Create( name, handler, desc, high_priority );
}

//---------------------------------------------------------------------------------------
Command::~Command() {
	m_inst->Remove( m_handler );
}

//---------------------------------------------------------------------------------------
bool TryExecuteCommand( const char *command_string ) {
	char name[64];
	Util::BreakString( command_string, name );
	Instance *inst;
	if( !g_command_trie.Get( name, inst ) ) {
		return false;
	}

	inst->Execute( command_string );
	return true;
}


void AddGlobalCommand( const char *name, Command::Handler handler, 
					  const char *desc, bool high_priority ) {
	

	g_global_commands.push_back( Command::Create( name, handler, desc, high_priority ) );
}

//---------------------------------------------------------------------------------------
}}
