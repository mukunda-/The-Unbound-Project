//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright � 2014, Mukunda Johnson, All rights reserved. =========//

#include <stdafx.h>

#include "system/system.h" 
#include "system/server/serverconsole.h"
#include "system/console.h"
#include "net/network_all.h"

class MyControl : public Net::Control {

};

class TestProgram : public System::Program {
	
	class NetEventHandler : public Net::EventHandler {

		TestProgram &m_parent;

		void Accepted( Net::Stream &stream ) {
			auto mc = static_cast<MyControl>(ctrl);

		}
		
	public:
		NetEventHandler( TestProgram &parent ) {
			m_parent = parent;
		}
	};

	Net::Listener<MyControl> m_listener;
	NetEventHandler m_events;
public:

	TestProgram() : m_events(*this), m_listener( m_events ) {

	}

	void OnStart() {
		m_listener.Start();
	}
};

void Test() {
	
}



void RunProgram() {

	Listener<MyControl> m_listener;

	System::RunProgram( TestProgram() );
	//std::thread
//	char buffer[25];
	//fgets(buffer,25,stdin);
	//System::ServerConsole::GetInput( buffer, sizeof buffer );
//	boost::this_thread::sleep_for( boost::chrono::seconds(15));

	//System::Join();
}

	 

//-------------------------------------------------------------------------------------------------
void Main( int argc, char *argv[] ) { 
	System::Instance i_system(2); 
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