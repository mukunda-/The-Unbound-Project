//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

#pragma once

#include "system/forwards.h"
#include "util/argstring.h"
#include "util/stringref.h"

//-----------------------------------------------------------------------------
namespace System { 
//-----------------------------------------------------------------------------

enum {
	COMMAND_CONTINUE,
	COMMAND_STOP
};

/// -----------------------------------------------------------------------
/// A Command is created for each Command handler existing in the command
/// execution system.
///
/// One or more commands are associated with a single command Instance.
///
class Command {

public:

	// handler type
	using Handler = std::function< void( Util::ArgString & )>;
	using ptr     = std::unique_ptr<Command>;

private:

	// shared with other commands with same name.
	std::shared_ptr< Commands::Instance > m_inst;

	// handler for this command.
	Handler m_handler;

	// unique id for instance management
	int m_id;

	int ID() { return m_id; }
	friend class Instance;
	
protected:

	Command( Util::StringRef name, Util::StringRef desc, 
			 Handler handler, bool high_priority );

public:
	~Command();

	// non-copyable, non-moveable
	Command( Command& )  = delete;
	Command( Command&& ) = delete;
	Command& operator=( Command& )  = delete;
	Command& operator=( Command&& ) = delete;
	
	/// -------------------------------------------------------------------
	/// Construct a new Command.
	///
	/// @param name    Name of command/the command trigger.
	/// @param desc    Brief description of command.
	/// @param handler Command handler to be called when the command is
	///                executed.
	/// @param high_priority 
	///                If an Instance already exists, add the 
	///                command handler to the beginning of the handler
	///                list, otherwise it is appended.
	/// 
	static ptr Create( Util::StringRef name, Util::StringRef desc, 
					   Handler handler, bool high_priority = false ) {

		return ptr( new Command( name, desc, handler, high_priority ));
	}

	void Execute( Util::ArgString &args );
};

/// ---------------------------------------------------------------------------
/// Try to execute a command string.
///
/// @param command_string Command line to execute.
/// @return true if the command was executed. false if there was no handler 
///         for it.
///
bool TryExecuteCommand( Util::StringRef command_string );

/// ---------------------------------------------------------------------------
/// Add a global command.
///
/// This adds a permanent command which cannot be removed later.
///
/// @param name Name of command, the command trigger.
/// @param desc Brief description of command.
/// @param handler Command handler to handle this command.
/// @param high_priority If an existing command instance is found, this
///                      handler will take priority over any existing
///                      handlers.
///
void AddGlobalCommand( Util::StringRef name, Util::StringRef desc, 
					   Command::Handler handler, 
					   bool high_priority = false );

//-----------------------------------------------------------------------------
}
