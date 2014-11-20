//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

#include "stdafx.h"
#include "system/system.h"
#include "system/console.h"
#include "system/commands.h"

#pragma warning( disable : 4996 )

namespace System {

Instance *g_instance;

//-----------------------------------------------------------------------------
namespace {
	
	//-------------------------------------------------------------------------
	int Command_Quit( Util::ArgString &args ) {

		System::Shutdown(); 
		return 0;
	}

}
  
//-----------------------------------------------------------------------------
Service::Service() {
	using namespace boost::asio;
	m_dummy_work = std::unique_ptr<io_service::work>( 
		new io_service::work( m_io_service ) );
}

//-----------------------------------------------------------------------------
Service::~Service() {
	Finish( true );
	Stop();
}

//-----------------------------------------------------------------------------
void Service::Finish( bool wait ) {
	// delete work object
	m_dummy_work.reset( nullptr ); 
	if( wait ) m_threads.join_all();
}

//-----------------------------------------------------------------------------
void Service::Run( int count ) {
	for(; count--; ) {
		
		m_threads.create_thread( 
			boost::bind( &boost::asio::io_service::run, &m_io_service )); 
	}
}

//-----------------------------------------------------------------------------
void Service::Join() {
	m_io_service.run();
}

//-----------------------------------------------------------------------------
void Service::Stop() {
	// terminate io service
	m_io_service.stop();
	m_threads.join_all();
} 

//-----------------------------------------------------------------------------
void Service::Post( std::function<void()> handler, int delay ) {

	if( delay == 0 ) {
		m_io_service.post( handler );

	} else {
		std::shared_ptr<boost::asio::deadline_timer> timer( 
				new boost::asio::deadline_timer(
					m_io_service, 
					boost::posix_time::milliseconds( delay ) ));

		timer->async_wait( 
			boost::bind( &Service::PostDelayedHandler, 
						 boost::asio::placeholders::error, 
						 timer, handler ));
	}
}

//-----------------------------------------------------------------------------
void Service::PostDelayedHandler( 
					    const boost::system::error_code &error, 
						std::shared_ptr<boost::asio::deadline_timer> &timer,
						std::function<void()> &handler ) {									
	if( !error ) 
		handler();
}
  
//-----------------------------------------------------------------------------
Service &GetService() {
	assert(g_instance);
	return g_instance->GetService();
}

//-----------------------------------------------------------------------------
void Finish() {
	GetService().Finish( true ); 
}

//-----------------------------------------------------------------------------
void RunProgram( Program &program ) {
	g_instance->RunProgram( program );
}

//-----------------------------------------------------------------------------
void Shutdown() {
	System::Console::Print( "Shutting down..." );
	g_instance->Shutdown();
}

//-----------------------------------------------------------------------------
void Log( const char *format, ... ) {
	va_list argptr;
	va_start(argptr, format);
	char text[4096];
	vsnprintf( text, sizeof text, format, argptr );
	System::Console::PrintS( text );
	// todo: log to file
	va_end(argptr);
}

//-----------------------------------------------------------------------------
void LogError( const char *format, ... ) {
	va_list argptr;
	va_start(argptr, format);
	char text[4096];
	vsnprintf( text, sizeof text, format, argptr );
	System::Console::PrintS( text );
	// todo: log to error file
	va_end(argptr);
}

//-----------------------------------------------------------------------------
bool Live() {
	return g_instance->Live();
}

//-----------------------------------------------------------------------------
void Join() {
	g_instance->GetService().Join();
}

//-----------------------------------------------------------------------------
void Post( std::function<void()> handler, bool main, int delay ) {
	g_instance->PostSystem( handler, main, delay );
}

//-----------------------------------------------------------------------------
Instance::Instance( int threads ) : m_strand( m_service() ) {
	assert( g_instance == nullptr );
	g_instance = this;
	m_service.Run( threads );
	m_live = true;

	System::Console::AddGlobalCommand( "quit", Command_Quit );
}

//-----------------------------------------------------------------------------
Instance::~Instance() {
	m_live = false;
	m_service.Finish( true );

	g_instance = nullptr;
}

//-----------------------------------------------------------------------------
void Instance::PostSystem( std::function<void()> handler, 
						   bool main, int delay ) {

	if( main ) {
		m_service.Post( m_strand.wrap( handler ), delay );
	} else {
		m_service.Post( handler, delay );
	}
}

//-----------------------------------------------------------------------------
void Instance::RunProgram( Program &program ) {
	m_program = &program;
	PostSystem( std::bind( &Program::OnStart, &program ) );
	System::Join();
}

//-----------------------------------------------------------------------------
Service &Instance::GetService() {
	return m_service;
}

//-----------------------------------------------------------------------------
void Instance::Shutdown() {
	m_live = false;
	m_service.Finish( false );
}

}

