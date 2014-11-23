//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//

#include "stdafx.h"
#include "commands.h"
#include "util/trie.h"
#include "util/argstring.h"

//-----------------------------------------------------------------------------
namespace System { namespace Console {
	
//-----------------------------------------------------------------------------
namespace {
	
	// the command map
	Util::Trie< Instance* > g_command_trie;

	// "global" commands are commands that are added in a simpler fashion
	// and cannot be removed.
	std::vector<Command::Pointer> g_global_commands;
}

/** ---------------------------------------------------------------------------
 * An Instance is created per each unique command name.
 *
 * An Instance contains one or more command handlers. When a command is issued
 * the instance passes the command string to all of the handlers in a sequence.
 *
 * The handlers have a choice to break the execution sequence.
 */
class Instance {

private:

	// name of command (also the trigger string)
	std::string m_name;

	// short description of command
	std::string m_desc;
	
	// handler list
	std::vector<Command::Handler> m_handlers;

	typedef std::shared_ptr<Instance> Pointer;

	/** -----------------------------------------------------------------------
	 * Construct a new instance.
	 *
	 * \param name Name of command, the command trigger.
	 * \param desc Brief description of command.
	 */
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

	/** -----------------------------------------------------------------------
	 * Add a new handler.
	 *
	 * Handlers are execute in a sequence when a command fires.
	 * 
	 * \param highprio If true, the handler will be inserted into the
	 *                 beginning of the handler list, so it gets
	 *                 processed before any existing handlers.
	 */
	void AddHandler( Command::Handler handler, bool highprio ) {
		if( highprio ) {
			m_handlers.insert( m_handlers.begin(), handler );
		} else {
			m_handlers.push_back( handler );
		}
	}

public:	
	
	~Instance() {
		// instances are auto destructed by the magic shared pointers.

		// unregister this instance.
		g_command_trie.Reset( m_name.c_str() );
	}

	/** -----------------------------------------------------------------------
	 * Get an Instance for a command.
	 *
	 * This looks up a command name, and if an Instance already exists, the 
	 * handler is added to it. Otherwise, a new Instance is created with the 
	 * handler and description.
	 *
	 * \param name     Name of command (command trigger)
	 * \param handler  Command handler
	 * \param desc     Brief description of command. This is only used if a new
	 *                 instance is created.
	 * \param highprio Only used if an Instance already exists, causes the new
	 *                 handler to be inserted at the beginning of the handler
	 *                 list instead of the end.
	 * \return         Pointer to new or existing Instance.
	 */
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

	/** -----------------------------------------------------------------------
	 * Remove a handler from the handler list.
	 *
	 * \param handler Command handler to remove.
	 */
	void Remove( Command::Handler handler ) {
		for( auto i = m_handlers.begin(); i != m_handlers.end(); i++ ) {
			if( *i == handler ) {
				m_handlers.erase( i );
				return;
			}
		}
	}

	/** -----------------------------------------------------------------------
	 * Check if there are any handlers for this Instance.
	 *
	 * \return true if the handler list is not empty.
	 */
	bool HasHandlers() {
		return m_handlers.size() != 0;
	}

	/** -----------------------------------------------------------------------
	 * Execute a command string with this Instance.
	 *
	 * \param cmdstring String to parse and pass to the command handlers.
	 */
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

//---------------------------------------------------------------------------------------
void AddGlobalCommand( const char *name, Command::Handler handler, 
					  const char *desc, bool high_priority ) {
	

	g_global_commands.push_back( Command::Create( name, handler, desc, high_priority ) );
}

//---------------------------------------------------------------------------------------
}}
