//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

#include <stdafx.h>
#include "listener.h"

namespace Net {


Listener::Listener( unsigned short port, 
				    std::function<Stream::ptr()> &factory, 
				    Event::Handler::ptr &handler )

		: BasicListener(port) {

	m_factory = factory;
	m_handler = handler;
}

void Listener::Start() {
	Stream::ptr stream = m_factory();
	m_event_handler.Subscribe( *stream );

}


}