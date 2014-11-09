//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

#include "basiclistener.h"
#include "netevents.h"

namespace Net {

	class Stream;
	typedef std::shared_ptr<Stream> StreamPtr;

	class Listener : public BasicListener {

		std::function<StreamPtr()> m_factory;

		Events::Stream::Handler m_event_handler;
		
	public:
		Listener( unsigned short port, 
				  std::function<StreamPtr()> &factory, 
				  Events::Stream::Handler &handler );

		void Start();
		void Stop();
	};

}
