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

class ServerNode;
ServerNode *g_main;

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

/// ---------------------------------------------------------------------------
/// Main class for the Server Node program
///
class ServerNode  {
	// connection to master
	Network::Connection m_cmaster; 

	// String sysvar containing the master's address
	System::Variable &sv_master_address;

	class NetworkEventHandler : public Network::Connection::EventHandler {

		void Connected( Network::Connection &connection ) override {
			System::Console::Print( "Connection to master established." );
			g_main->state = STATE_CONNECTED;
		}

		void ConnectError( Network::Connection &connection,
				const boost::system::error_code &error ) {
			
			System::LogError( "Couldn't connect to master \"%s\". Quitting.", 
				connection.GetHostname().c_str() );
			g_shutdown = true;
		}

	};

	NetworkEventHandler net_event_handler;

	enum {
		STATE_CONNECTING,
		STATE_CONNECTED
	};
	
	int state;
	 
public:

	ServerNode() : 
			sv_master_address(
				System::Variable::Create( 
					"sv_master_address", 
					System::Variable::T_STRING, 
					"localhost", "Address of master server" )
			)
	{
		state = STATE_CONNECTING;
		System::Console::AddGlobalCommand( "quit", Command_Quit, "Shutdown server" );
	
		m_cmaster.SetEventHandler( &net_event_handler );
		
	}

	//-------------------------------------------------------------------------------------------------
	static int Command_Quit( Util::ArgString &args ) {
		g_shutdown = true;
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
  
/// ---------------------------------------------------------------------------
/// Create main instance and execute it
///
void RunProgram() {
	auto instance = std::unique_ptr<ServerNode>( new ServerNode );
	g_main = instance.get();
	g_main->Run();
}

/// ---------------------------------------------------------------------------
/// Program entry point
///
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
