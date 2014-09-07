//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//

#include <stdafx.h>
#include "util/keyvalues.h"
#include "util/trie.h"
#include "system/system.h"
#include "system/console.h"
#include "system/commands.h"
#include "system/ServerConsole.h"
#include "util/linkedlist.h"
#include "network/connection.h"

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
/*
typedef boost::function< int( int ) > CommandHandler;
//typedef int (*CommandHandler)( int client, int args );
Util::Trie<CommandHandler> commands;

void RegisterCommand( const char *name, CommandHandler handler ) {
	commands.Set( name, handler, true );
}*/
 
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

	Client()  {

	}
	 
};

//-------------------------------------------------------------------------------------------------
class Zone : 
		public Util::LinkedItem<Zone>   {

public:
	  
	Network::Connection m_net;
	 
};

//-------------------------------------------------------------------------------------------------
class ServerMaster {
	

private: 

	Util::LinkedList<Client> m_clients; 
	Network::Listener m_client_listener;
	std::unique_ptr<Client> m_new_client;

	Util::LinkedList<Zone> m_zones;
	Network::Listener m_zone_listener;
	std::unique_ptr<Zone> m_new_zone; // current "listening" zone
		
	  
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
	ServerMaster() : m_client_listener( 32798 ), m_zone_listener( 32790 ) {
		
		AcceptClient();
		System::Console::Print( "Now accepting clients." );
		 
		AcceptZone();
		System::Console::Print( "Now accepting zone services." );


	}

	//-------------------------------------------------------------------------------------------------
	~ServerMaster() {

		for( Client *c = m_clients.GetFirst(); c; ) {
			Client *next = c->m_next;
			delete c;
			c = next;
		}  

		for( Zone *zone = m_zones.GetFirst(); zone; ) {
			Zone *next = zone->m_next;
			delete zone;
			zone = next;
		}
	}
	 
	//4745781458114587478-5045781-4578-114578-84571258-1258-12-01270-1250-12580-1240-1245780-71240-124570-1240-1250-1245780-12450-120-12450-12450-1245-124580-14570-1258428-1458-20712458-1470-8518-05-028-05-0714570-000-258-258-521258218-28-2-2525825-25-25-252514-25-25-523821448148-2518-1248-1245
	//-------------------------------------------------------------------------------------------------
	void AcceptClient() {

		m_new_client = std::unique_ptr<Client>( new Client );
		m_new_client->m_net.SetUserData( m_new_client.get() );
		m_new_client->m_net.SetEventHandler( 
			boost::bind( &ServerMaster::OnClientEvent, this, _1, _2, _3 ) ); 
		m_new_client->m_net.Listen( m_client_listener );
	}

	//-------------------------------------------------------------------------------------------------
	void AcceptZone() { 
		m_new_zone = std::unique_ptr<Zone>( new Zone ); 
		m_new_zone->m_net.SetUserData( m_new_zone.get() );
		m_new_zone->m_net.SetEventHandler( 
			boost::bind( &ServerMaster::OnZoneEvent, this, _1,_2,_3 ) );
		m_new_zone->m_net.Listen( m_zone_listener );
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
	int OnZoneEvent( Network::Connection &source, Network::Connection::EventType type, void *data ) {
		switch( type ) {
			case Network::Connection::EVENT_ACCEPTEDCONNECTION: {
				System::Log( "Zone server connected from \"%s\"", 
					m_new_zone->m_net.Socket().remote_endpoint().address().to_string().c_str() );
				

				m_zones.Add( m_new_zone.release() );
				AcceptZone();
				return 0;
			} case Network::Connection::EVENT_ACCEPTERROR: {
				const boost::system::error_code *error = (const boost::system::error_code*)data;
				System::Log( "Error accepting zone server: Code %d", *error );
				AcceptZone();
				return 0;

			}
		}
		return 0;
	}
	
	int OnClientEvent( Network::Connection  &source, Network::Connection::EventType type, void *data ) {
		Client &c = *(source.GetUserData<Client>());
		if( g_shutdown ) return 0;

		switch( type ) {
			case Network::Connection::EVENT_ACCEPTEDCONNECTION: {

				System::Log( "Client connected from TODO" );
				m_clients.Add( m_new_client.release() );
				AcceptClient();

				return 0;
			} case Network::Connection::EVENT_ACCEPTERROR: {
				const boost::system::error_code *error = (const boost::system::error_code*)data;
				System::Log( "Error accepting client: Code %d", *error );
				AcceptClient();
				return 0;

			} case Network::Connection::EVENT_RECEIVE: {
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
				}

				return 1;
			} case Network::Connection::EVENT_DISCONNECT:
			case Network::Connection::EVENT_DISCONNECT2: {
				if( !c.m_nickname.empty() ) {
					System::Console::PrintEx( "%s has left.", c.m_nickname.c_str() ); 
				}
				RemoveClient( &c );
				UpdateClientListing();
			}

		}
		
		return 0;
	}
	 
	void Run() {
		System::Console::AddGlobalCommand( "quit", Command_Quit );
		System::Post( IOThread );

		/*
		while( !g_shutdown ) {
			System::Console::GetInput( input, sizeof input );
			Util::BreakString( input, cmd, sizeof cmd );
			Util::StringToUpper( cmd );
			Util::StringASCIIFilter( cmd );
			System::Console::Print( ">>> %s", input );
			
			if( cmd[0] != 0 ) {
				CommandHandler handler;
				if( commands.Get( cmd, handler ) ) {
					handler( 0  );
				} else {
					System::Console::PrintError( "Unknown command: %s", cmd );
			
				}
			}

			System::ServerConsole::Update(); 
		}*/

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


int main( int argc, char *argv[] ) {
	Main( argc, argv );
	return 0;
}
