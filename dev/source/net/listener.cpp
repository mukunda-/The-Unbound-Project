//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright � 2014, Mukunda Johnson, All rights reserved. =========//

#include <stdafx.h>
#include "listener.h"

namespace Net {


Listener::Listener( unsigned short port, 
				    std::function<*Stream()> factory, 
				    EventHandler handler )

		: BasicListener(port) {


	 
}

void Listener::Start() {

}


}