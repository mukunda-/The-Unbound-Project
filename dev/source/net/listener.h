//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright � 2014, Mukunda Johnson, All rights reserved. =========//

#include "basiclistener.h"
#include "eventhandler.h"

namespace Net {

	class Listener : public BasicListener {

		//Stream::ptr m_new_stream;
	public:
		Listener( unsigned short port, 
				  std::function<*Stream()> factory, 
				  EventHandler handler );

		void Start();
		void Stop();
	};

}
