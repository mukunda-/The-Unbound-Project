//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

#include <stdafx.h>
#include "eventlatch.h"

namespace Net {

EventLatch::EventLatch( EventHandler &handler ) 
	: std::lock_guard( handler.m_latch ) {
	
}

}
