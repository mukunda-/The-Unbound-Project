//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

#include <stdafx.h>

#include "system/system.h"
#include "network/network_all.h"
#include "system/server/serverconsole.h"

#define VERSION "DEV1"
#define WINDOW_TITLE "UNBOUND SERVER (AUTH) " VERSION

bool g_shutdown;

void RunProgram() {
	
}

//-------------------------------------------------------------------------------------------------
void Main( int argc, char *argv[] ) {
	System::Init i_system(1);
	Network::Init i_network(1);
	{
		System::ServerConsole::Init i_serverconsole;

		System::ServerConsole::SetTitle( WINDOW_TITLE );

		g_shutdown = false;
		RunProgram(); 
	}
	printf(" OK.");
	getc(stdin);
}

//-------------------------------------------------------------------------------------------------
int main( int argc, char *argv[] ) {
	Main( argc, argv );
	return 0;
}
