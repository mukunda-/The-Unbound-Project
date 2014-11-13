//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

#include <stdafx.h>
#include "authserver.h"
#include "protocol.h"
#include "auth_login.pb.h"

namespace User {

	//-------------------------------------------------------------------------
	AuthStream::AuthStream() {
		m_state = STATE_LOGIN;
	}

	//-------------------------------------------------------------------------
	AuthServer::NetEventHandler::NetEventHandler( AuthServer &parent ) 
		: m_parent(parent) 
	{}
	
	//-------------------------------------------------------------------------
	void AuthServer::NetEventHandler::AcceptError( 
							Net::Stream::ptr &stream, 
							const boost::system::error_code &error ) {
			
		System::Log( "A connection failed to accept." );
		m_parent.m_listener.Start();
	}

	//-------------------------------------------------------------------------
	void AuthServer::NetEventHandler::Accepted( 
			Net::Stream::ptr &stream ) {
	}

	//-------------------------------------------------------------------------
	void AuthServer::NetEventHandler::Receive( 
				Net::Stream::ptr &source, 
				Net::Remsg &msg ) {

		AuthStream &stream = static_cast<AuthStream&>(*source); 
		if( stream.Invalidated() ) return;
		if( stream.GetState() == AuthStream::STATE_LOGIN ) {
		
			if( msg.ID() == Net::Proto::Messages::LOGIN ) {
				// user wants to log in.
				Net::Proto::
			} else {
				// bad client.
				stream.SetState( AuthStream::STATE_DONE );
			}
		}
	}

	//-------------------------------------------------------------------------
	AuthServer::AuthServer() : m_event_handler(*this), 
				   m_listener( 32791, StreamFactory, &m_event_handler )
	{
	}

	//-------------------------------------------------------------------------
	AuthServer::~AuthServer() {
		m_event_handler.Disable();
		m_listener.Stop();
	}

	//-------------------------------------------------------------------------
	void AuthServer::OnStart() {

		System::Console::Print( "Listening." );
		m_listener.Start();
	}

	//-------------------------------------------------------------------------
	Net::Stream::ptr AuthServer::StreamFactory() {
		return std::make_shared<AuthStream>();
	}
}
