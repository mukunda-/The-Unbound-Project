//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

#include "stdafx.h"
#include "service.h"

//-----------------------------------------------------------------------------
namespace System {

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
	if( !error ) {
		handler();
	}
}

//-----------------------------------------------------------------------------
}