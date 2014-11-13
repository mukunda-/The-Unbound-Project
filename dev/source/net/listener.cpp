//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

#include <stdafx.h>
#include "stream.h"
#include "listener.h"
#include "nwcore.h"

namespace Net {
	
	//-------------------------------------------------------------------------
	Listener::Listener( unsigned short port, 
						std::function<StreamPtr()> factory, 
						Events::Stream::Handler *handler ) :
			BasicListener(port), 
			m_factory( factory ), 
			m_user_handler(handler),
			m_accept_handler(*this)  { 
	}

	//-------------------------------------------------------------------------
	Listener::~Listener() {
		Stop();
		m_accept_handler.Disable();
	}

	//-------------------------------------------------------------------------
	void Listener::Start() {
		std::lock_guard<std::mutex> lock(m_mutex);
		if( m_started ) return; // already started 
		m_started = true;
		Accept();
	}

	//-------------------------------------------------------------------------
	void Listener::Stop() {
		std::lock_guard<std::mutex> lock(m_mutex);
		m_started = false; 
	}

	//-------------------------------------------------------------------------
	void Listener::SetIgnoreErrors( bool ignore ) {
		std::lock_guard<std::mutex> lock(m_mutex);
		m_ignore_errors = ignore;
	}

	//-------------------------------------------------------------------------
	void Listener::Accept() {
		if( m_current_stream ) return; // already listening.
		m_current_stream = m_factory();
		m_current_stream->AsevSubscribe( m_accept_handler );
		if( m_user_handler ) {
			m_current_stream->AsevSubscribe( *m_user_handler );
		}
		m_current_stream->Listen( *this );
	} 

	//-------------------------------------------------------------------------
	void Listener::OnCompleted() {
		
		std::lock_guard<std::mutex> lock(m_mutex);
		m_current_stream = nullptr;
		if( m_started ) Accept(); 
	}

	//-------------------------------------------------------------------------
	void Listener::OnError() {
		std::lock_guard<std::mutex> lock(m_mutex);
		m_current_stream = nullptr;
		if( !m_ignore_errors ) {
			m_started = false;
			return;
		}
		if( m_started ) Accept();

	}

	//-------------------------------------------------------------------------
	Listener::EventHandler::EventHandler( Listener &parent ) 
			: m_parent(parent) {
	}

	//-------------------------------------------------------------------------
	void Listener::EventHandler::Accepted( StreamPtr &stream ) {
		stream->GetService().Post( 
				boost::bind( 
					&Listener::OnCompleted, &m_parent ) );
	}

	//-------------------------------------------------------------------------
	void Listener::EventHandler::AcceptError( 
					StreamPtr &stream, 
					const boost::system::error_code &error ) {
		stream->GetService().Post( 
				boost::bind( 
					&Listener::OnError, &m_parent ) );
	}
}