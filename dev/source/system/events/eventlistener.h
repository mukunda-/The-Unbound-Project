//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2015, Mukunda Johnson, All rights reserved. =========//

#pragma once

#include "event.h"

//-----------------------------------------------------------------------------
namespace System {

/** ---------------------------------------------------------------------------
 * An interface for objects to listen for events which handles automatic
 * hook cleanup.
 */
class EventListener final {

	std::vector<int> m_hook_ids;

public:
	EventListener();
	~EventListener();

	template< typename T >
	int Hook( Event::Handler handler ) {
		return Hook( T::CODE, handler );
	}

	int Hook( uint32_t code, Event::Handler handler );
};

}
