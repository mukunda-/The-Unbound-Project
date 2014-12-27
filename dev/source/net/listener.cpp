//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

#include <stdafx.h>
#include "stream.h"
#include "listener.h"
#include "core.h"

namespace Net {
//-----------------------------------------------------------------------------

using errcode = const boost::system::error_code;

//-----------------------------------------------------------------------------
class Listener::EventHandler : public Events::Stream::Handler {

	Listener &m_parent;

public:
	void Accepted( StreamPtr &stream ) override;
	void AcceptError( StreamPtr &stream, errcode &error ) override;

	EventHandler( Listener &parent );
};
	
//-----------------------------------------------------------------------------
Listener::Listener( unsigned short port,
					StreamFactory factory,
					Events::Stream::Handler::ptr handler ) :
		BasicListener(port),
		m_factory( factory ),
		m_user_handler(handler) {
		
	m_accept_handler = std::make_shared<EventHandler>( *this );
	Accept();
}
	 
//-----------------------------------------------------------------------------
Listener::~Listener() {}

//-----------------------------------------------------------------------------
void Listener::Accept() {
	auto stream = m_factory();

	stream->AsevSubscribe( m_accept_handler );
	stream->AsevSubscribe( m_user_handler );

	stream->Listen( *this );
}

//-----------------------------------------------------------------------------
Listener::EventHandler::EventHandler( Listener &parent ) 
		: m_parent(parent) {
}

//-----------------------------------------------------------------------------
void Listener::EventHandler::Accepted( StreamPtr &stream ) {

	stream->AsevUnsubscribe( m_parent.m_accept_handler );
	m_parent.Accept();
}

//-----------------------------------------------------------------------------
void Listener::EventHandler::AcceptError( StreamPtr &stream, errcode& ) {

	stream->AsevUnsubscribe( m_parent.m_accept_handler );
	m_parent.Accept();
}

//-----------------------------------------------------------------------------
}