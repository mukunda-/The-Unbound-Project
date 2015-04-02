//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

// Authentication Server

#include <stdafx.h>

#include "system/system.h" 
#include "system/server/serverconsole.h"
#include "programs/auth/authserver.h"
#include "net/core.h"

#define VERSION "DEV1"
#define WINDOW_TITLE "UNBOUND SERVER (AUTH) " VERSION
 
//-----------------------------------------------------------------------------
void Main( int argc, char *argv[] ) {
	System::Main i_system(4);
	System::ServerConsole::Instance i_serverconsole( WINDOW_TITLE );
	System::RegisterModule<Net::Instance>(1);
	System::RegisterModule<User::AuthServer>();

	System::Start();
}

//-----------------------------------------------------------------------------
int main( int argc, char *argv[] ) {

	Main( argc, argv );
	google::protobuf::ShutdownProtobufLibrary();
	return 0;
}
