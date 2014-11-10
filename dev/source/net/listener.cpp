//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

#include <stdafx.h>
#include "stream.h"
#include "listener.h"
#include "nwcore.h"

namespace Net {
	
	//----------------------------------------------------------------------------
	Listener::Listener( unsigned short port, 
						std::function<StreamPtr()> factory, 
						Events::Stream::Handler *handler ) :
			BasicListener(port), 
			m_factory( factory ), 
			m_user_handler(handler),
			m_accept_handler(*this) {
		
	}

	//----------------------------------------------------------------------------
	void Listener::Start() {
		std::lock_guard<std::mutex> lock(m_mutex);
		if( m_current_stream.get() ) return; // already started.

		m_current_stream = m_factory();
		m_current_stream->AsevSubscribe( m_accept_handler );
		if( m_user_handler ) m_current_stream->AsevSubscribe( *m_user_handler );
		m_current_stream->Listen( *this );
	}

	//----------------------------------------------------------------------------
	void Listener::Stop() {
		
	}
}