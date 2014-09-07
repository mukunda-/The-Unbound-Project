//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//

#include "stdafx.h"
#include "system/system.h"
#include "system/console.h"

namespace System {

//static std::vector<std::thread> g_threads;
 
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
/*
void CleanThreadList() {
	// this seems nasty
	for( auto i = g_threads.begin(); i != g_threads.end();) {
		auto a = i++;
		if( !(*a).joinable() ) {
			g_threads.erase(a);
		}
	}
}*/
/*
void test() {
}*/

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

