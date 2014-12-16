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
	using Handler = std::function< int( Util::ArgString & ) >;

private:

	// shared with other commands with same name.
	std::shared_ptr< Commands::Instance > m_inst;

	// handler for this command.
	Handler m_handler;

	// unique id for instance management
	int m_id;
	
public:

	typedef std::unique_ptr<Command> ptr;
	
	/// -------------------------------------------------------------------
	/// Construct a new Command.
	///
	/// @param name    Name of command/the command trigger.
	/// @param handler Command handler to be called when the command is
	///                executed.
	/// @param desc    Brief description of command.
	/// @param high_priority 
	///                If an Instance already exists, add the 
	///                command handler to the beginning of the handler
	///                list, otherwise it is appended.
	/// 
	Command( Util::StringRef name, Handler handler, 
			 Util::StringRef desc, bool high_priority );

	~Command();

	// non-copyable, non-moveable
	Command( Command& ) = delete;
	Command( Command&& ) = delete;
	Command& operator=( Command& ) = delete;
	Command& operator=( Command&& ) = delete;

	// helper function to create a new command pointer
	static ptr Create( Util::StringRef name, Handler handler, 
					   Util::StringRef desc, bool high_priority ) {

		return ptr( new Command( name, handler, desc, high_priority ));
	}
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
/// @param handler Command handler to handle this command.
/// @param desc Brief description of command.
/// @param high_priority If an existing command instance is found, this
///                      handler will take priority over any existing
///                      handlers.
///
void AddGlobalCommand( Util::StringRef name, Command::Handler handler, 
					   Util::StringRef desc = "", 
					   bool high_priority = false );

//-----------------------------------------------------------------------------
}
