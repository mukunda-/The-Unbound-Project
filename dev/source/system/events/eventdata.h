//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2015, Mukunda Johnson, All rights reserved. =========//

#pragma once

namespace System {

/** ---------------------------------------------------------------------------
 * Holds event info and a handler list for registered events.
 */
class EventData {

public:
	using Handler = std::function< void( Event &e ) >;

	EventData( const EventInfo &info );

	/** -----------------------------------------------------------------------
	 * Add an event handler for this event.
	 *
	 * @param Handler Event handler function.
	 * @returns Handler ID.
	 */
	int AddHandler( Handler handler );

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
	const EventInfo      &m_info;
	std::vector<Handler>  m_handlers; 
};

}
