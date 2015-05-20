//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2015, Mukunda Johnson, All rights reserved. =========//

#pragma once

#include "event.h"

namespace System {

/** ---------------------------------------------------------------------------
 * Handles a single event hook.
 */
class EventHook {
	
	Event::Handler m_handler;
	
public:
	EventHook( Event::Handler &handler ) : m_handler(handler) {}

	void operator()( Event &e ) { m_handler(e); }
};

}