//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//

#pragma once

#include "util/argstring.h"

//---------------------------------------------------------------------------------------
namespace System { namespace Console {
	
//---------------------------------------------------------------------------------------
class Instance;

enum {
	COMMAND_CONTINUE,
	COMMAND_STOP
};

/// ---------------------------------------------------------------------------
/// A Command is created for each Command handler existing in the command
/// execution system.
///
/// One or more commands are associated with a single command Instance.
///
class Command {

public:

	// handler type
	typedef int (*Handler)( Util::ArgString &args );

private:

	// shared with other commands with same name.
	std::shared_ptr<Instance> m_inst;

	// handler for this command.
	Handler m_handler;
	
	// non-copyable.
	Command( Command& );
public:

	typedef std::shared_ptr<Command> Pointer;
	
	/// -----------------------------------------------------------------------
	/// Construct a new Command.
	///
	/// \param name Name of command, the command trigger.
	/// \param handler Command handler to be called when the command is
	///                executed
	/// \param desc Brief description of command.
	/// \param high_priority Only used if an Instance already exists, causes 
	///                      the new handler to be inserted at the beginning 
	///                      of the handler list instead of the end.
	/// 
	Command( const char *name, Handler handler, const char *desc = nullptr, bool high_priority = false );
	~Command();

	// helper function to create a new command pointer
	static Pointer Create( const char *name, Handler handler, 
							const char *desc = nullptr, bool high_priority = false ) {

		return Pointer( new Command( name, handler, desc, high_priority ) );
	}
};

/// ---------------------------------------------------------------------------
/// Try to execute a command string.
///
/// \param command_string Command line to execute.
/// \return true if the command was executed. false if there was no handler 
///         for it.
///
bool TryExecuteCommand( const char *command_string );

/// ---------------------------------------------------------------------------
/// Add a global command.
///
/// This adds a permanent command which cannot be removed later.
///
/// \param name Name of command, the command trigger.
/// \param handler Command handler to handle this command.
/// \param desc Brief description of command.
/// \param high_priority If an existing command instance is found, this
///                      handler will take priority over any existing
///                      handlers.
///
void AddGlobalCommand( const char *name, Command::Handler handler, 
					  const char *desc = nullptr, bool high_priority = false );

//-----------------------------------------------------------------------------
}}
