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
	friend class Listener;
	Listener *m_parent;

public:
	void Accepted( StreamPtr &stream ) override;
	void AcceptError( StreamPtr &stream, errcode &error ) override;

	EventHandler( Listener &parent );
};
	
//-----------------------------------------------------------------------------
Listener::Listener( unsigned short port, StreamFactory factory ) :
		BasicListener(port),
		m_factory( factory ) {
		
	m_accept_handler = std::make_shared<EventHandler>( *this );
	Accept();
}
	 
//-----------------------------------------------------------------------------
Listener::~Listener() {
	std::lock_guard<std::recursive_mutex> lock( m_accept_handler->GetMutex());
	auto &handler = static_cast<Listener::EventHandler&>(*m_accept_handler);
	handler.m_parent = nullptr;
}

//-----------------------------------------------------------------------------
void Listener::Accept() {
	auto stream = m_factory();

	stream->AsevSubscribe( m_accept_handler );
	stream->Listen( *this );
}

//-----------------------------------------------------------------------------
Listener::EventHandler::EventHandler( Listener &parent )
		: m_parent(&parent) {
}

//-----------------------------------------------------------------------------
void Listener::EventHandler::Accepted( StreamPtr &stream ) {
	
	std::this_thread::sleep_for( std::chrono::milliseconds( 1000 ) );
	stream->AsevUnsubscribe( *this );
	if( m_parent ) m_parent->Accept();
}

//-----------------------------------------------------------------------------
void Listener::EventHandler::AcceptError( StreamPtr &stream, errcode& ) {

	stream->AsevUnsubscribe( *this );
	if( m_parent ) m_parent->Accept();
}

//-----------------------------------------------------------------------------
}