//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

#include <stdafx.h>
#include "authserver.h"

namespace User {

	AuthStream::AuthStream() {
		m_state = STATE_LOGIN;
	}

	AuthServer::NetEventHandler::NetEventHandler( AuthServer &parent ) 
		: m_parent(parent) 
	{}
	
	void AuthServer::NetEventHandler::AcceptError( 
							Net::Stream::ptr &stream, 
							const boost::system::error_code &error ) {
			
		System::Log( "A connection failed to accept." );
	}

	void AuthServer::NetEventHandler::Accepted( 
			Net::Stream::ptr &stream ) {

	}

	AuthServer::AuthServer() : m_event_handler(*this), 
				   m_listener( 32791, StreamFactory, &m_event_handler )
	{
	}

	void AuthServer::OnStart() {

		System::Console::Print( "Listening." );
		m_listener.Start();
	}

	Net::Stream::ptr AuthServer::StreamFactory() {
		return std::make_shared<AuthStream>();
	}
}
