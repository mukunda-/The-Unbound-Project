//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

#include <stdafx.h>
#include "eventhandler.h"

namespace Net {

EventHandler() {
	m_disabled = false;
}

void EventHandler::Disable() {
	EventLatch( *this );
	m_disabled = true;
}

}