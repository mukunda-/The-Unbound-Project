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

//---------------------------------------------------------------------------------------
class Command {

public:
	typedef int (*Handler)( Util::ArgString &args );

private:
	std::shared_ptr<Instance> m_inst;
	Handler m_handler;
	
	Command( Command& );
public:

	typedef std::shared_ptr<Command> Pointer;
	Command( const char *name, Handler handler, const char *desc = nullptr, bool high_priority = false );
	~Command();

	static Pointer Create( const char *name, Handler handler, 
							const char *desc = nullptr, bool high_priority = false ) {

		return Pointer( new Command( name, handler, desc, high_priority ) );
	}
};

bool TryExecuteCommand( const char *command_string );

void AddGlobalCommand( const char *name, Command::Handler handler, 
					  const char *desc = nullptr, bool high_priority = false );

//---------------------------------------------------------------------------------------
}}
