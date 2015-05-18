//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2015, Mukunda Johnson, All rights reserved. =========//

#pragma once

//-----------------------------------------------------------------------------
namespace System {

/** ---------------------------------------------------------------------------
 * An interface for objects to listen for events.
 */
class EventListener {

	std::vector<int> m_hooked_event_ids;

protected:
	EventListener();

public:

	void Hook( 
};

}