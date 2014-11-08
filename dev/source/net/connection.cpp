//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

#include <stdafx.h>
#include "connection.h"
#include "resolver.h"

namespace Net {


	/*

//-------------------------------------------------------------------------------------------------
Connection::Connection() : m_stream( new Stream(this) ) {
	m_event_handler = nullptr;
	m_userdata = nullptr;
}*/
	/*
//-------------------------------------------------------------------------------------------------
Connection::~Connection() {
	m_stream->Shutdown();
} */
 /*
//-------------------------------------------------------------------------------------------------
void Connection::Listen( Listener &listener ) {
	listener.AsyncAccept( m_stream->m_socket, boost::bind( 
			&Stream::OnAccept, m_stream->shared_from_this(), 
			boost::asio::placeholders::error ) );
}*/
	/*
//-------------------------------------------------------------------------------------------------
void Connection::Connect( const std::string &host, const std::string &service ) {
	Resolver resolver; 
	m_hostname = host + ":" + service;
	boost::system::error_code ec;
	boost::asio::connect( m_stream->m_socket, resolver.Resolve( host, service ) );
	m_stream->m_connected = true;

	if( m_event_handler ) {
		m_event_handler->Connected( *this );
	}
}
*/
	/*
//-------------------------------------------------------------------------------------------------
void Connection::ConnectAsync( const std::string &host, const std::string &service ) {
	m_hostname = host + ":" + service;
	Resolver::CreateThreaded( host, service, 
		boost::bind( 
			&Stream::OnResolve, m_stream->shared_from_this(), _1, _2 ) ); 
}
*/

  


//-------------------------------------------------------------------------------------------------
void Connection::Stream::Write( Packet *p ) {
	m_send_fifo.Push(p);
	{
		boost::lock_guard<boost::mutex> lock(m_send_lock);
		if( m_sending ) return;
		m_send_read = 0;
		m_send_write = 0;
		m_send_packet = 0;
		m_sending = true;
		ContinueSend();
	}
}

//-------------------------------------------------------------------------------------------------
void Connection::Write( Packet *p ) { 

	m_stream->Write( p ); 
}

//-------------------------------------------------------------------------------------------------
void Connection::SetEventHandler( EventHandler *handler ) {
	m_event_handler = handler;
}

//-------------------------------------------------------------------------------------------------
const std::string &Connection::GetHostname() const {
	return m_hostname;
}

}
