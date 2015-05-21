//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2015, Mukunda Johnson, All rights reserved. =========//

#pragma once

#include "system/forwards.h"
#include "event.h"

//-----------------------------------------------------------------------------
namespace System {

/** ---------------------------------------------------------------------------
 * Holds event info and a handler list for registered events.
 */
class EventData final {

	friend class EventInterface;

public:
	/** -----------------------------------------------------------------------
	 * Create an event data instance.
	 *
	 * @param id   ID from event interface.
	 * @param info Pointer to event info.
	 */
	EventData( const EventInfo &info );
	~EventData();

	/** -----------------------------------------------------------------------
	 * Add an event hook to this event.
	 *
	 * @param Handler Event handler function.
	 * @returns Handler ID.
	 */
	void AddHook( EventHookPtr &hook );

	/** -----------------------------------------------------------------------
	 * Send an event to all registered handlers.
	 *
	 * @param e Event.
	 */
	void Accept( Event &e );

	//-------------------------------------------------------------------------
private:
	using WeakHook = std::weak_ptr<EventHook>;

	const EventInfo      &m_info;

	std::vector<WeakHook> m_hooks; 
};

}
