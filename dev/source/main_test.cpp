//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

#include <stdafx.h>

#include "system/system.h" 
#include "system/server/serverconsole.h"
#include "system/console.h"
#include "net/stream.h"
#include "net/listener.h"
#include "net/core.h"

#include "protocol.h"
#include "proto/auth/login.pb.h"

#include "util/trie.h"

#include "db/core.h"
#include "db/endpoint.h"

#include "mem/arena.h"


class MyStream : public Net::Stream {

};

class TestProgram : public System::Program {
	
	class NetEventHandler : public Net::Events::Stream::Handler {

		TestProgram &m_parent;
		
			void ConnectError( Net::Stream::ptr &stream,
							   const boost::system::error_code &error ) {
				System::Console::Print( "Couldn't connect : %s", error.message().c_str() );
			}
			 
			virtual void Connected( Net::Stream::ptr &stream ) {
				namespace PID = Net::Proto::ID;
				
				System::Console::Print( "Connected... Sending login." );
				Net::Proto::Auth::Login message;
				message.set_username( "testes" );
				message.set_password( "password" );
				stream->Write( Net::PBMsg( PID::Auth::LOGIN, message ) );
				stream->Close();
			}


	public:
		NetEventHandler( TestProgram &parent ) : m_parent(parent) {
		}
	};

	NetEventHandler m_events;
public:
	
	static Net::Stream::ptr myfactory() {
		return std::make_shared<MyStream>();
	}

	TestProgram() : 
			m_events(*this) {

	}

	~TestProgram() {
		m_events.Disable();
	}

	void OnStart() {

		Net::ConnectAsync( "localhost", "32791", m_events );
	}
};

void Test() {
	
}




void RunProgram() {
	//std::allocator<int> poop;
	
	YAML::Node config = YAML::LoadFile("private/sql.yaml");
		
	DB::Endpoint info;
	info.m_address = config["address"].as<std::string>();
	info.m_username = config["user"].as<std::string>();
	info.m_password = config["password"].as<std::string>();
	info.m_database = config["database"].as<std::string>();

	//DB::Register( std::make_unique<DB::Connection>( "test", info ) );
	
	
}

	 

//-------------------------------------------------------------------------------------------------
void Main( int argc, char *argv[] ) { 
	System::Instance i_system(2); 
	Net::Instance i_net(2);
	DB::Instance i_db(1);
	{
	//	setvbuf(stdin, NULL, _IONBF, 0); //turn off buffering
	//	getchar();
		
		//boost::asio::windows::stream_handle poop( System::GetService()(), 
		//			  GetStdHandle(STD_INPUT_HANDLE) );
		System::ServerConsole::Instance i_serverconsole( "TESTING" );
		RunProgram(); 
	} 
	
}

//-------------------------------------------------------------------------------------------------
int main( int argc, char *argv[] ) {
	Main( argc, argv );
	return 0;
}

/*

boost::asio::io_service test_service;

void pee( const char *text ) {
	printf("hi");
}
 
void PostDelayedHandler( 
					const boost::system::error_code &error, 
					std::shared_ptr<boost::asio::deadline_timer> &timer,
					std::function<void()> &handler ) {

	if( !error ) {
		handler();
	}
}

void PostDelayed( std::function<void()> handler, int delay ) {
	
	std::shared_ptr<boost::asio::deadline_timer> timer( 
			new boost::asio::deadline_timer(
							test_service, 
							boost::posix_time::milliseconds( delay ) ));

	timer->async_wait( boost::bind( PostDelayedHandler, 
						boost::asio::placeholders::error, timer, handler ));
}


*/