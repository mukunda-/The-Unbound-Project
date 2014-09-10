//===========================  The Unbound Project  =========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

// [SERVER] NODE

#include "stdafx.h"
#include "system/server/ServerConsole.h"
#include "system/variables.h"
#include "system/console.h"
#include "system/commands.h"
#include "system/system.h"

#include "network/connection.h"

//-------------------------------------------------------------------------------------------------

#pragma comment( lib, "libprotobuf.lib" )

//-------------------------------------------------------------------------------------------------
#define VERSION "DEV1.3"

#define WINDOW_TITLE ("UNBOUND SERVER (NODE) " VERSION)

volatile bool g_shutdown;

class ServerZone;
ServerZone *g_main;

//-------------------------------------------------------------------------------------------------
void IOThread() {
	char input[256];
	while( !g_shutdown ) {
		System::ServerConsole::GetInput( input, sizeof input );

		System::Console::Execute( input );

		System::ServerConsole::Update();
		
		if( g_shutdown ) break;
	}
}

//-------------------------------------------------------------------------------------------------
class ServerZone  {
	Network::Connection m_cmaster; // connection to master
	System::Variable &sv_master_address;

	enum {
		STATE_CONNECTING
	};
	
	int state;
	 
public:

	ServerZone() : 
			sv_master_address(System::Variable::Create( "sv_master_address", System::Variable::T_STRING, 
							"localhost", "Address of master server" ))
	{
		state = STATE_CONNECTING;
		System::Console::AddGlobalCommand( "quit", Command_Quit, "Shutdown server" );
	
		m_cmaster.SetEventHandler( boost::bind( &ServerZone::OnNetworkEvent, this, _1, _2, _3 ) );
		
	}

	//-------------------------------------------------------------------------------------------------
	static int Command_Quit( Util::ArgString &args ) {
		g_shutdown = true;
		return 0;
	}

	
	//-------------------------------------------------------------------------------------------------
	int OnNetworkEvent( Network::Connection &source, Network::Connection::EventType type, void *data ) {
		switch( state ) {
			case STATE_CONNECTING: {
				if( type == Network::Connection::EVENT_CONNECTERROR ) {
					System::LogError( "Couldn't connect to master \"%s\". Quitting.", source.GetHostname().c_str() );
					g_shutdown = true;

				} else if( type == Network::Connection::EVENT_CONNECTED ) {
					System::Console::Print( "Connected to master established." );
					// todo: change state
				}
				break;
			}
		}
		return 0;
	}

	void Run() {
		
		System::Console::ExecuteScript( "cfg/server.cfg" );
		System::Console::ExecuteScript( "debug.cfg" );
		System::Post( IOThread );
		
		System::Console::Print( "---" );
		System::Console::Print( "Connecting to master..." );
		m_cmaster.ConnectAsync( sv_master_address.GetString().c_str(), "32790" );

		while( !g_shutdown ) {
			std::this_thread::sleep_for( std::chrono::milliseconds(5) );
		}
	}
};

 

//boost::asio::io_service test;
//boost::asio::ip::tcp::socket s(test); 

// main class

//-------------------------------------------------------------------------------------------------
void RunProgram() {
	auto instance = std::unique_ptr<ServerZone>( new ServerZone );
	g_main = instance.get();
	g_main->Run();
}

//-------------------------------------------------------------------------------------------------
int main() {
	
	System::Init i_system(2);
	Network::Init i_network(1);
	
	{
		System::ServerConsole::Init i_serverconsole;
		g_shutdown = false;
		
		System::ServerConsole::SetTitle( WINDOW_TITLE );
		System::ServerConsole::Update();
		
		RunProgram();   


	}
	return 0;
}
