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
 
/// ---------------------------------------------------------------------------
/// Main class for the Server Node program
///
class ServerNode : public System::Program {
	// connection to master
	Network::Connection m_cmaster; 

	// String sysvar containing the master's address
	System::Variable &sv_master_address;

	class NetworkEventHandler : public Network::Connection::EventHandler {
		ServerNode &m_parent;

		void Connected( Network::Connection &connection ) override {
			System::Console::Print( "Connection to master established." );
			m_parent.state = STATE_CONNECTED;
		}

		void ConnectError( Network::Connection &connection,
				const boost::system::error_code &error ) override  {
			
			System::LogError( "Connection failed, retrying in 120 seconds." );
			System::Post( 
				std::bind(&ServerNode::RetryConnection, &m_parent), true , 5*1000 );
		}
		
	public:
		NetworkEventHandler( ServerNode &parent ) : m_parent(parent) {}
	};

	NetworkEventHandler net_event_handler;

	enum {
		STATE_CONNECTING,
		STATE_CONNECTED
	};
	
	int state;

	void RetryConnection() {
		Connect();

	}

	void Connect() {
		System::Console::Print( "Connecting to master..." );
		m_cmaster.ConnectAsync( sv_master_address.GetString().c_str(), "32790" );	
	}
	 
public:

	ServerNode() : 
			sv_master_address(
				System::Variable::Create( 
					"sv_master_address", 
					System::Variable::T_STRING, 
					"localhost", "Address of master server" )
			),
			net_event_handler( *this )
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
	 

	void OnStart() override {
		
		System::Console::ExecuteScript( "cfg/server.cfg" );
		System::Console::ExecuteScript( "debug.cfg" );
		
		System::Console::Print( "---" );
		Connect();
	}
};
   

/// ---------------------------------------------------------------------------
/// Program entry point
///
int main() {
	
	System::Instance i_system(4);
	Network::Init i_network(1);
	
	{
		System::ServerConsole::Instance i_serverconsole( WINDOW_TITLE );
		
		auto instance = std::unique_ptr<ServerNode>( new ServerNode );
		System::RunProgram( *instance );

	}
	return 0;
}
