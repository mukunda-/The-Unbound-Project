//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

#include "stdafx.h"
#include "commands.h"
#include "system.h"
#include "util/argstring.h"

//-----------------------------------------------------------------------------
namespace System {

extern Main *g_main;

namespace Commands {

	/** ---------------------------------------------------------------------------
	 * An Instance is created per each unique command name.
	 *
	 * An Instance contains one or more command handlers. When a command is issued
	 * the instance passes the command string to all of the handlers in a sequence.
	 *
	 * The handlers have a choice to break the execution sequence.
	 */
	class Instance : public std::enable_shared_from_this<Instance> {

	private:

		// name of command (also the trigger string)
		std::string m_name;

		// short description of command
		std::string m_desc;
	
		// handler list
		std::vector<Command*> m_handlers;

		using ptr = std::shared_ptr<Instance>;
	

		/** -----------------------------------------------------------------------
		 * Add a new handler.
		 *
		 * Handlers are execute in a sequence when a command fires.
		 * 
		 * \param highprio If true, the handler will be inserted into the
		 *                 beginning of the handler list, so it gets
		 *                 processed before any existing handlers.
		 */
		void AddHandler( Command &handler, bool highprio );
		
		/** -------------------------------------------------------------------
		 * Remove a handler from the handler list.
		 *
		 * @param handler Command handler to remove.
		 */
		void RemoveHandler( Command &handler );

		friend class Command;

	public:
		~Instance();

		/** -----------------------------------------------------------------------
		 * Construct a new instance.
		 *
		 * @param name Name of command, the command trigger.
		 * @param desc Brief description of command.
		 */
		Instance( Util::StringRef name, Util::StringRef desc );

		/** -------------------------------------------------------------------
		 * Get an Instance for a command.
		 *
		 * This looks up a command name, and if an Instance already exists, 
		 * the handler is added to it. Otherwise, a new Instance is created 
		 * with the handler and description.
		 *
		 * @param name     Name of command (command trigger)
		 * @param handler  Command handler
		 * @param desc     Brief description of command. This is only used 
		 *                 if a new instance is created.
		 * @param highprio Only used if an Instance already exists, causes 
		 *                 the new handler to be inserted at the beginning 
		 *                 of the handler list instead of the end.
		 *
		 * @returns Pointer to new or existing Instance.
		 */
		static ptr Create( Util::StringRef name, Command &handler, 
						   Util::StringRef desc, bool highprio );

		/** -------------------------------------------------------------------
		 * Execute a command string with this Instance.
		 *
		 * @param cmdstring String to parse and pass to the command handlers.
		 */
		void Execute( Util::StringRef cmdstring );
	};

	//-------------------------------------------------------------------------
	auto Instance::Create( Util::StringRef name, Command &handler, 
						   Util::StringRef desc, bool highprio ) -> ptr {

		auto inst = g_main->FindCommandInstance( name );
		if( inst ) {
			inst->AddHandler( handler, highprio );
			return inst;
		}
		
		inst = std::make_shared<Instance>( name, desc );
		inst->AddHandler( handler, highprio );
		return ptr( inst );
	}
	
	//-------------------------------------------------------------------------
	void Instance::AddHandler( Command &handler, bool highprio ) {
		if( highprio ) {
			m_handlers.insert( m_handlers.begin(), &handler );
		} else {
			m_handlers.push_back( &handler );
		}
	}

	//-------------------------------------------------------------------------
	void Instance::RemoveHandler( Command &handler ) {
		for( auto i = m_handlers.begin(); i != m_handlers.end(); i++ ) {
			if( *i == &handler ) {
				m_handlers.erase( i );
				return;
			}
		}

		assert( !"Tried to remove nonexistant handler." );
	}

	//-------------------------------------------------------------------------
	void Instance::Execute( Util::StringRef cmdstring ) {
		Util::ArgString args( cmdstring );

		for( auto handler : m_handlers ) {
		
			handler->Execute( args );	 
		}
	}

	//-------------------------------------------------------------------------
	Instance::Instance( Util::StringRef name, Util::StringRef desc ) {
		// this class is not created directly
		// new instance, we push it into the trie,
		assert( !name.Empty() );
		 
		m_desc = desc;
		m_name = name;

		assert( g_main->m_command_map.count(name) == 0 );

		// push the new instance into the trie
		// as soon as all references to this are deleted
		// the destructor will pull it from the trie.
		g_main->m_command_map[ name ] = this;
	}

	//-------------------------------------------------------------------------
	Instance::~Instance() {
		// instances are destructed after all handlers are deleted.

		// unregister this instance.
		g_main->m_command_map.erase( m_name );
	}
}

//---------------------------------------------------------------------------------------
Command::Command( Util::StringRef name, Util::StringRef desc, 
				  Handler handler, bool high_priority ) {

	m_id = g_main->AllocCommandID();
	m_handler = handler;
	m_inst = Commands::Instance::Create( name, *this, desc, high_priority );
}

//---------------------------------------------------------------------------------------
Command::~Command() {
	m_inst->RemoveHandler( *this );
}

//---------------------------------------------------------------------------------------
void Command::Execute( Util::ArgString &args ) {
	m_handler( args );
}
	
//---------------------------------------------------------------------------------------
bool ExecuteCommand( Util::StringRef command_string ) {
	return g_main->TryExecuteCommand( command_string );
}

//---------------------------------------------------------------------------------------
bool Main::TryExecuteCommand( Util::StringRef command_string ) {

	char name[64];
	Util::BreakString( *command_string, name );

	auto inst = FindCommandInstance(name);
	if( !inst ) return false;

	inst->Execute( command_string );
	return true;
}

//---------------------------------------------------------------------------------------
void AddGlobalCommand( Util::StringRef name, Util::StringRef desc, 
					   Command::Handler handler, bool high_priority ) {
	
	g_main->SaveCommand( Command::Create( name, desc, handler, high_priority ));
}

//---------------------------------------------------------------------------------------
Commands::InstancePtr Main::FindCommandInstance( const Util::StringRef &name ) {
	try {
		return m_command_map.at( name )->shared_from_this();
	} catch( const std::out_of_range& ) {}
	return nullptr;
}

//---------------------------------------------------------------------------------------
void Main::SaveCommand( CommandPtr &&cmd ) {
	m_global_commands.push_back( std::move( cmd ));
}

}
