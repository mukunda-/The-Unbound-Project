//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

#include <stdafx.h>
#include "stream.h"
#include "listener.h"

namespace Net {
	 
	Listener::Listener( unsigned short port, 
						std::function<StreamPtr()> factory, 
						Events::Stream::Handler *handler ) :
			BasicListener(port), 
			m_factory( factory ), 
			m_event_handler(handler) {
		
	}

	void Listener::Start() {
		Stream::ptr stream = m_factory();
		if( m_event_handler ) stream->AsevSubscribe( *m_event_handler );
	}


}