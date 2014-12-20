//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

#include <stdafx.h>
#include "stream.h"
#include "listener.h"
#include "core.h"

namespace Net {
	
	//-------------------------------------------------------------------------
	Listener::Listener( unsigned short port, 
						StreamFactory factory, 
						Events::Stream::Handler *handler ) :
			BasicListener(port), 
			m_factory( factory ), 
			m_user_handler(handler),
			m_accept_handler(*this)  { 
		
		Accept();
	}
	 
	//-------------------------------------------------------------------------
	Listener::~Listener() {
		m_accept_handler.Disable();
	}

	//-------------------------------------------------------------------------
	void Listener::Accept() {
		auto stream = m_factory();

		stream->AsevSubscribe( m_accept_handler );
		if( m_user_handler ) {
			stream->AsevSubscribe( *m_user_handler );
		}
		stream->Listen( *this );
	}

	//-------------------------------------------------------------------------
	Listener::EventHandler::EventHandler( Listener &parent ) 
			: m_parent(parent) {
	}

	//-------------------------------------------------------------------------
	void Listener::EventHandler::Accepted( StreamPtr &stream ) {

		m_parent.Accept();
	}

	//-------------------------------------------------------------------------
	void Listener::EventHandler::AcceptError( 
					StreamPtr &stream, 
					const boost::system::error_code &error ) {

		m_parent.Accept();
	}
}