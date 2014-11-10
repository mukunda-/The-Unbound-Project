//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

// Authentication Server

#include <stdafx.h>

#include "system/system.h" 
#include "system/server/serverconsole.h"
#include "programs/auth/authserver.h"
#include "net/nwcore.h"

#define VERSION "DEV1"
#define WINDOW_TITLE "UNBOUND SERVER (AUTH) " VERSION
 
//-----------------------------------------------------------------------------
void Main( int argc, char *argv[] ) {
	System::Instance i_system(1);
	Net::Instance i_network(1);
	{
		System::ServerConsole::Instance i_serverconsole( WINDOW_TITLE );
		System::RunProgram( User::AuthServer() );
	}
}

//-----------------------------------------------------------------------------
int main( int argc, char *argv[] ) {
	Main( argc, argv );
	return 0;
}
