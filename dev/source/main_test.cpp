//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

#include <stdafx.h>

#include "system/system.h" 
#include "system/server/serverconsole.h"
#include "system/console.h"
 

void Test() {
	
}

void RunProgram() {
	
	//std::thread
//	char buffer[25];
	//fgets(buffer,25,stdin);
	//System::ServerConsole::GetInput( buffer, sizeof buffer );

	while(1){
		boost::this_thread::sleep_for( boost::chrono::seconds(1));
	}
}

//-------------------------------------------------------------------------------------------------
void Main( int argc, char *argv[] ) {
	System::Init i_system(2); 
	{
	//	setvbuf(stdin, NULL, _IONBF, 0); //turn off buffering
	//	getchar();
		
		//boost::asio::windows::stream_handle poop( System::GetService()(), 
		//			  GetStdHandle(STD_INPUT_HANDLE) );
		System::ServerConsole::Instance i_serverconsole( "TESTING" );
		RunProgram(); 
	} 
	getc(stdin);
}

//-------------------------------------------------------------------------------------------------
int main( int argc, char *argv[] ) {
	Main( argc, argv );
	return 0;
}
