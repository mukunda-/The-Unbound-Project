//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//

#include "stdafx.h"

using boost::asio::ip::tcp;

//-------------------------------------------------------------------------------------------------
bool OnConnectionEvent( Network::Connection &source, Network::Connection::EventType type, void *data ) {

	//System::Console::Print( p.data );
	return true;
}
/*
void IOThread( boost::asio::io_service *service ) {
	
	service->run();
}*/

//-------------------------------------------------------------------------------------------------
int main( int argc, char *argv[] ) {

	Network::DEFAULT_SERVICE.Run(1);

	System::Console::Init();
	System::Console::SetTitle( "client test" );

	System::Console::Print( "Hello, Planet Earth.\n\nEnter Ctrl+[ to disconnect.\n" ); 
	
	char input[1024];
	std::string nickname;

	System::Console::Print( "Please enter a nickname.");
	System::Console::Update(); 
	System::Console::GetInputEx( "Nickname: ", input, 32 );
	nickname = input;
	System::Console::Print( "Connecting..." );
	System::Console::SetTitle( input );
	System::Console::Update();

	Network::Connection client;
  
	try { 
		 
		client.SetEventHandler( OnConnectionEvent );
		client.Connect( "localhost", "32798" ); 
		
		client.Write( Network::BuildPacket( "\x01%s",  nickname.c_str() ) );
		client.WaitSendComplete();

		System::Console::Print( "Connected!" );
		System::Console::Update(); 

		while(true) {
			
			System::Console::GetInput( input, 256 );
			
			if( input[0] == 27 ) {
				System::Console::Print( "Goodbye!" ); 
				break;
			}
			Util::StripASCIIControls( input );
			System::Console::Print( ">>> %s", input ); 
			client.Write( Network::BuildPacket( "\x02%s", input ) );

			System::Console::Update(); 

		}
		client.Socket().close();
		  
	} catch( std::exception &e ) {
		System::Console::PrintError( "%s", e.what() );
	}

	Network::DEFAULT_SERVICE.Stop();
	System::Console::Update();
	getc( stdin );
	System::Console::End();
	return 0;
}
