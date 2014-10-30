//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

// [SERVER] MASTER

#include <stdafx.h>
#include "util/keyvalues.h"
#include "util/trie.h"
#include "system/system.h"
#include "system/console.h"
#include "system/commands.h"
#include "system/server/ServerConsole.h"
#include "util/linkedlist.h"
#include "network/connection.h"
//-------------------------------------------------------------------------------------------------

#pragma comment( lib, "libprotobuf.lib" )

//-------------------------------------------------------------------------------------------------

#define VERSION "DEV1.3"

#define WINDOW_TITLE ("UNBOUND SERVER (MASTER) " VERSION)

class ServerMaster;
bool g_shutdown;
ServerMaster *g_main = nullptr;

//-------------------------------------------------------------------------------------------------
int Command_Quit( Util::ArgString &args ) {
	
	g_shutdown = true;
	return 0;
}

#if 0
int Command_Test(  int args ) {
	/*Util::KeyValues kv;
	kv.LoadFromFile( "testees.txt" );
	
	kv.Rewind(); 
	System::Console::Print( "\nroot" );

	PrintKV( kv, 0 );*/

#if 0
	try {
		boost::asio::ip::tcp::resolver resolver(*io_service);
	
		boost::asio::ip::tcp::resolver::query query( "www.google.com", "http" );
		boost::asio::ip::tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
		boost::asio::ip::tcp::socket socket(*io_service);

		boost::asio::connect( socket, endpoint_iterator );

		boost::asio::streambuf request;
		std::ostream request_stream(&request);
		request_stream << "GET /index.html HTTP/1.0\r\n";
		request_stream << "Host: www.gogle.com\r\n";
		request_stream << "Accept: */*\r\n";
		request_stream << "Connection: close\r\n\r\n";

		boost::asio::write( socket, request );
		
		for( ;; ) {
			boost::array<char,64> buf;
			char buf2[129];
			buf2[128] = 0;
			boost::system::error_code error;

			size_t len = socket.read_some(boost::asio::buffer(buf) , error);
			memcpy( buf2, buf.c_array(), len );
			buf2[len] = 0;
			//buf[len] = 0;
			System::Console::Print( "%s" , buf2 );

			if (error == boost::asio::error::eof) {
				
				
				break; // Connection closed cleanly by peer.
			} else if (error) {
				throw boost::system::system_error(error); // Some other error.
			}
			
		}
	} catch (std::exception& e) {
		System::Console::PrintError( "Error: %s", e.what() );
	}
#endif
	return 0;

}
#endif
 
void PrintKV( Util::KeyValues &kv, int level ) {
	 
	if( !kv.GotoFirstKey( false ) ) {
		kv.Exit();
		return;
	}

	char name[256];
	kv.GetSectionName( name, sizeof name );
	System::Console::Print( "\n" );
	for( int i = 0; i < level-1; i++ ) {
		System::Console::Print( "  " );
	}
	System::Console::Print( name  );

	do {

		
		if( kv.Enter() ) {
			PrintKV( kv, level+1 );
		} else {
			
			kv.GetKeyName( name, sizeof name );
			if( name[0] != 0 ) {
				char value[256];
				System::Console::Print( "\n" );
				
				for( int i = 0; i < level; i++ ) {
					System::Console::Print( "  " );
					
				}
				kv.GetString( 0, value, sizeof value );
				
				System::Console::Print( "%s %s",name, value  );
				
			}
		}

	} while( kv.GotoNextKey(false) );
	

	kv.Exit();
}
 
 
//-------------------------------------------------------------------------------------------------
void IOThread() {
	char input[256];
	while( !g_shutdown ) {
		System::ServerConsole::GetInput( input, sizeof input );
		if( g_shutdown ) break;
		System::Console::Execute( input );

		System::ServerConsole::Update();
		
		if( g_shutdown ) break;
	}
}

//-------------------------------------------------------------------------------------------------
class Client : 
		public Util::LinkedItem<Client>  {

public:
	std::string m_nickname;

	Network::Connection m_net;

	int m_state;

	enum {
		STATE_LOGIN
	};

	Client()  {
		m_state = STATE_LOGIN;
	}
	 
};

//-------------------------------------------------------------------------------------------------
class Node  {

public:
	
	Network::Connection m_net;
	
};

//-------------------------------------------------------------------------------------------------
class ServerMaster {
	

private: 

	class NodeEventHandler : public Network::Connection::EventHandler {
		
		ServerMaster &m_parent;

		void AcceptedConnection( Network::Connection &connection) override {
			System::Log( "Node connected from \"%s\"", 
					connection.Socket()
					.remote_endpoint().address().to_string().c_str() );
				
			m_parent.m_nodes.push_back( std::move( m_parent.m_new_node ) );
			m_parent.AcceptNode();
		}

		void AcceptError( Network::Connection &connection,
				const boost::system::error_code &error  ) override {

			System::Log( "Error accepting node server: Code %d", *error );
			m_parent.AcceptNode();

		}

		void Disconnected( Network::Connection &connection, 
				const boost::system::error_code &error ) override {


		}

		void DisconnectedError( Network::Connection &connection,
				const boost::system::error_code &error ) override {

		}
		
	public:

		NodeEventHandler( ServerMaster &parent ) : m_parent(parent) {}
	};

	class ClientEventHandler : public Network::Connection::EventHandler {
		ServerMaster &m_parent;

	public:
		
		ClientEventHandler( ServerMaster &parent ) : m_parent(parent) {}

		
		void AcceptedConnection( Network::Connection &connection) override {
			
			System::Log( "Client connected from TODO" );
			m_parent.m_clients.Add( m_parent.m_new_client.release() );
			m_parent.AcceptClient();
		}

		void AcceptError( Network::Connection &connection,
				const boost::system::error_code &error  ) override {

			System::Log( "Error accepting client: Code %d", *error );
			m_parent.AcceptClient();
		}

		void Disconnected( Network::Connection &connection, 
				const boost::system::error_code &error ) override {

			Client &c = *(connection.GetUserData<Client>());
			if( !c.m_nickname.empty() ) {
				System::Console::PrintEx( "%s has left.", c.m_nickname.c_str() ); 
			}
			m_parent.RemoveClient( &c );
			m_parent.UpdateClientListing();
		}

		void DisconnectedError( Network::Connection &connection,
				const boost::system::error_code &error ) override {

			Disconnected( connection, error );
		}

		bool Receive( Network::Connection &connection,
				Network::Packet &packet ) {

			// TODO.
				/*
			Network::Packet *p = (Network::Packet*)data;
				if( p->size == 0 ) return 1;
				switch((int)p->data[0] ) {
				case 1:
					// nickname
					if( c.m_nickname.empty() ) {
						c.m_nickname = std::string( p->data+1, p->size-1 );
						System::Log( "%s has connected.", c.m_nickname.c_str() ); 
						UpdateClientListing();
					} else {
						RemoveClient(&c); // client is being naughty.
					}
					break;
				case 2:
					// message
					std::string message( p->data+1, p->size-1);
					System::Console::Print( "[%s] %s", c.m_nickname.c_str(), message.c_str() ); 
				}*/

			return true;
		}
	};

	NodeEventHandler node_event_handler;
	ClientEventHandler client_event_handler;

	Util::LinkedList<Client> m_clients; 
	Network::Listener m_client_listener;
	std::unique_ptr<Client> m_new_client;

	std::vector<std::unique_ptr<Node>> m_nodes;
	//Util::LinkedList<Node> m_nodes;
	Network::Listener m_node_listener;
	std::unique_ptr<Node> m_new_node; // current "listening" node
		
	  
	//-------------------------------------------------------------------------------------------------
	Client * NewClient() {
		Client *c = new Client;
	}

	//-------------------------------------------------------------------------------------------------
	void RemoveClient( Client *c ) {
		m_clients.Remove( c );
		delete c;
	}
	
public:

	//-------------------------------------------------------------------------------------------------
	ServerMaster() : 
			m_client_listener( 32798 ), 
			m_node_listener( 32790 ),
			node_event_handler( *this ),
			client_event_handler( *this ) {
		
		AcceptClient();
		System::Console::Print( "Now accepting clients." );
		 
		AcceptNode();
		System::Console::Print( "Now accepting nodes." );


	}

	//-------------------------------------------------------------------------------------------------
	~ServerMaster() {

		for( Client *c = m_clients.GetFirst(); c; ) {
			Client *next = c->m_next;
			delete c;
			c = next;
		}  
		/*
		for( Node *node = m_nodes.GetFirst(); node; ) {
			Node *next = node->m_next;
			delete node;
			node = next;
		}*/
	}
	 
	//4745781458114587478-5045781-4578-114578-84571258-1258-12-01270-1250-12580-1240-1245780-71240-124570-1240-1250-1245780-12450-120-12450-12450-1245-124580-14570-1258428-1458-20712458-1470-8518-05-028-05-0714570-000-258-258-521258218-28-2-2525825-25-25-252514-25-25-523821448148-2518-1248-1245
	//-------------------------------------------------------------------------------------------------
	void AcceptClient() {

		m_new_client = std::unique_ptr<Client>( new Client );
		m_new_client->m_net.SetUserData( m_new_client.get() );
		m_new_client->m_net.SetEventHandler( &client_event_handler );
		m_new_client->m_net.Listen( m_client_listener );
	}

	//-------------------------------------------------------------------------------------------------
	void AcceptNode() { 
		m_new_node = std::unique_ptr<Node>( new Node ); 
		m_new_node->m_net.SetUserData( m_new_node.get() );
		m_new_node->m_net.SetEventHandler( &node_event_handler );
		m_new_node->m_net.Listen( m_node_listener );
	}

	//-------------------------------------------------------------------------------------------------
	void UpdateClientListing() {
		int line = 0; 
		for( Client *c = m_clients.GetFirst(); c; c = c->m_next ) {
			if( !c->m_nickname.empty() ) { 
				System::ServerConsole::SetMenuItem( line++, "%s", false, c->m_nickname.c_str() );
			}
		}  
		for( ; line < System::ServerConsole::SIDEBAR_HEIGHT; line++ ) {
			System::ServerConsole::SetMenuItem( line, "", false );
		}
		System::ServerConsole::Update();
	} 

	//------------------------------------------------------------------------------------------------- 
	void Run() {
		System::Console::AddGlobalCommand( "quit", Command_Quit );
		System::Post( IOThread );
		 

		while( !g_shutdown ) {
			std::this_thread::sleep_for( std::chrono::milliseconds(5) );
		}
	  
	}
};

//-------------------------------------------------------------------------------------------------
void RunProgram() {
	auto instance = std::unique_ptr<ServerMaster>( new ServerMaster );
	instance->Run();
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
