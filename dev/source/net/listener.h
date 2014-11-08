//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

#include "basiclistener.h"
#include "event.h"

namespace Net {

	class Listener : public BasicListener {

		std::function<Stream::ptr()> m_factory;
		Event::Handler::ptr m_event_handler;
		//Stream::ptr m_new_stream;
	public:
		Listener( unsigned short port, 
				  std::function<*Stream()> &factory, 
				  Event::Handler::ptr &handler );

		void Start();
		void Stop();
	};

}
