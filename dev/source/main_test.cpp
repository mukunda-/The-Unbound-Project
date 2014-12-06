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
#include "net/textstream.h"
#include "console/console.h"

#include "protocol.h"
#include "proto/auth/login.pb.h"

#include "util/trie.h"

#include "db/core.h"
#include "db/endpoint.h"
#include "db/querybuilder.h"
#include "db/statement.h"
#include "db/callbacktransaction.h"

#include "mem/arena/arena.h"

#include "util/slinkedlist.h"

#include "system/callback.h"
#include "util/stringles.h"
 
class MyStream : public Net::TextStream {

};

class TestProgram : public System::Program {
	
	class NetEventHandler : public Net::Events::Stream::Handler {

		TestProgram &m_parent;
		
			virtual void Accepted( Net::Stream::ptr &stream ) {
				Console::Print( "ACCEPTED" );
			}
			virtual void AcceptError( 
					Net::Stream::ptr &stream,
					const boost::system::error_code &error ) {
				Console::Print( "ACCEPTERROR", error.message().c_str() );
			}
			
			virtual void Disconnected( 
					Net::Stream::ptr &stream,
					const boost::system::error_code &error ) {
				Console::Print( "DISCONNECTED", error.message().c_str() );
			}
			virtual void SendFailed( 
					Net::Stream::ptr &stream,
					const boost::system::error_code &error ) {
				Console::Print( "SENDFAILED %s", error.message().c_str() );
			}
			void ConnectError( Net::Stream::ptr &stream,
							   const boost::system::error_code &error ) {
				Console::Print( "Couldn't connect : %s", error.message().c_str() );
			}
			/*
			virtual void Connected( Net::Stream::ptr &str ) {
				namespace PID = Net::Proto::ID;

				auto &stream = str->Cast<Net::LidStream>();
				
				System::Console::Print( "Connected... Sending login." );

				Net::Proto::Auth::Login message;
				message.set_username( "testes" );
				message.set_password( "password" );

				stream.Write() << PID::Auth::LOGIN << message; 
				stream.Close();
				System::Console::Print( "ok!" );
			}
			*/
			
			//-----------------------------------------------------------------
			virtual void Connected( Net::Stream::ptr &str ) {
				namespace PID = Net::Proto::ID;

				auto &stream = str->Cast<MyStream>();
				
				Console::Print( "Connected... Sending login." );
				 
				stream.Write().Formatted( "Hello %s", "world." );
				
				Console::Print( "ok!" );
			}

			//-----------------------------------------------------------------
			void Receive( Net::Stream::ptr &str, Net::Message &netmsg ) override {
				auto &msg = netmsg.Cast<Net::TextStream::Message>();
				
				Console::Print( "Received message: %s", msg().c_str() );
				
			}

	public:
		NetEventHandler( TestProgram &parent ) : m_parent(parent) {
		}
	};
	
	Net::Listener m_listener; 
	NetEventHandler m_events;
public:
	
	static Net::Stream::ptr myfactory() {
		return std::make_shared<MyStream>();
	}

	TestProgram() : m_events(*this),
			m_listener( 32791, StreamFactory, &m_events ) {

	}
	
	//-------------------------------------------------------------------------
	static Net::Stream::ptr StreamFactory() {
		return std::make_shared<MyStream>();
	}

	~TestProgram() {
		m_events.Disable();
		m_listener.Stop();
	}

	void OnStart() {
		
	//	m_listener.Start();
		/*
		YAML::Node config = YAML::LoadFile("private/sql.yaml");
	//	YAML::Node config = YAML::LoadFile("../test/test.yaml");
	//	System::Console::Print( "%s", config["address"].as<std::string>().c_str() );
		
		DB::Endpoint info;
		info.m_address  = config["address" ].as<std::string>();
		info.m_username = config[  "user"  ].as<std::string>();
		info.m_password = config["password"].as<std::string>();
		info.m_database = config["database"].as<std::string>();
		
		auto &con = DB::Register( "test", info );
		
		auto test = DB::TransactionPtr( 
				new TestX( DB::CallbackTransaction::Bind( &TestProgram::Test, this )));  
		con.Execute( test );*/
	//	Net::ConnectAsync( "localhost", "32791", m_events, MyStreamFactory );
		
		//f.operator%(5);
		//System::Console::Print( Util::Format( "testes %d %s %d %d", 535, "3235", 1, 2 ).c_str() );
		//System::Console::Print( (boost::format( "testes %d %s %d %d" )% 535 % "3235" % 1 % 2).str().c_str() );

		System::Log( "%s %s", "a", "b" );
		//System::Console::Print( f.str().c_str() );
	}

	static std::shared_ptr<MyStream> MyStreamFactory() {
		return std::make_shared<MyStream>();
	}

	void Test( std::shared_ptr<DB::Transaction> &t, bool failed ) {}
};
	 
sql::Connection *testes;

//-------------------------------------------------------------------------------------------------
void RunProgram() {
	System::RunProgram( TestProgram() );
	 
	//builder.AddEscapedString( "hi" );
	//builder % "hi";
}

//-------------------------------------------------------------------------------------------------
void Main( int argc, char *argv[] ) { 
	Console::Instance i_console;
	System::Instance i_system(2); 
	System::ServerConsole::Instance i_serverconsole( "TESTING" );
	Net::Instance i_net(1);
//	DB::Manager i_db(1);
	{
		
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