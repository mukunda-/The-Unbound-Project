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
	EventData( EventInfo &info );

	/** -----------------------------------------------------------------------
	 * Add an event handler for this event.
	 *
	 * @param Handler Event handler function.
	 * @returns Handler ID.
	 */
	int AddHandler( Event::Handler handler );

	/** -----------------------------------------------------------------------
	 * Unhook an event handler.
	 *
	 * @param id ID from AddHandler.
	 */
	void RemoveHandler( int id );
	
	/** -----------------------------------------------------------------------
	 * Send an event to all registered handlers.
	 *
	 * @param e Event.
	 */
	void Accept( Event &e );

	//-------------------------------------------------------------------------
private:
	EventInfo                   &m_info;
	std::vector<Event::Handler>  m_handlers; 
};

}
