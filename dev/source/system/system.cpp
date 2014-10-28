//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//

#include "stdafx.h"
#include "system/system.h"
#include "system/console.h"

#pragma warning( disable : 4996 )

namespace System {
 
Service *g_service = nullptr;

//-------------------------------------------------------------------------------------------------
Service::Service() {
	m_dummy_work = std::unique_ptr<boost::asio::io_service::work>( 
		new boost::asio::io_service::work( m_io_service ) );
}

//-------------------------------------------------------------------------------------------------
Service::~Service() {
	Finish();
	Stop();
}

void Service::Finish() {
	// delete work object
	m_dummy_work.reset( nullptr );

	m_threads.join_all();
}

//-------------------------------------------------------------------------------------------------
void Service::Run( int count ) {
	for(; count--; ) {
		
		m_threads.create_thread( boost::bind(&boost::asio::io_service::run, &m_io_service) );
		//boost::thread t( boost::bind( &Service::IOThread, this ) );
	}
}

//-------------------------------------------------------------------------------------------------
void Service::Stop() {
	m_io_service.stop();
	m_threads.join_all();
} 

void Service::PostDelayed( std::function<void()> handler, int delay )  {
			
	boost::shared_ptr<boost::asio::deadline_timer> timer( 
			new boost::asio::deadline_timer(
							m_io_service, 
							boost::posix_time::milliseconds( delay ) ));

	timer->async_wait( boost::bind( &Service::PostDelayedCallback, 
						boost::asio::placeholders::error, timer, handler ));
} 

void Service::PostDelayedCallback( 
					    const boost::system::error_code &error, 
						boost::shared_ptr<boost::asio::deadline_timer> &timer,
						std::function<void()> &handler ) {									
	if( !error ) {
		handler();
	}
}
 

//-------------------------------------------------------------------------------------------------
Service &GetService() {
	return *g_service;
}

//-------------------------------------------------------------------------------------------------
void Finish() {
	GetService().Finish(); 
}

//-------------------------------------------------------------------------------------------------
void Log( const char *format, ... ) {
	va_list argptr;
	va_start(argptr, format);
	char text[4096];
	vsnprintf( text, sizeof text, format, argptr );
	System::Console::PrintS( text );
	// todo: log to file
	va_end(argptr);
}

//-------------------------------------------------------------------------------------------------
void LogError( const char *format, ... ) {
	va_list argptr;
	va_start(argptr, format);
	char text[4096];
	vsnprintf( text, sizeof text, format, argptr );
	System::Console::PrintS( text );
	// todo: log to error file
	va_end(argptr);
}

//-------------------------------------------------------------------------------------------------
Init::Init( int threads ) {
	assert( g_service == nullptr );
	g_service = new Service();

	g_service->Run( threads );
}

//-------------------------------------------------------------------------------------------------
Init::~Init() {
	g_service->Finish();
	delete g_service;
	g_service = nullptr;
}

}

