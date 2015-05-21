//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2015, Mukunda Johnson, All rights reserved. =========//

#pragma once

#include "system/forwards.h"

//-----------------------------------------------------------------------------
namespace System {

/** ---------------------------------------------------------------------------
 * Internal class used to encapsulate the event system.
 */
class EventInterface final {

	using EventDataPtr = std::unique_ptr<EventData>;
	std::unordered_map< std::string, EventDataPtr > m_map;

public:
	EventInterface();
	~EventInterface();

	// register an event
	void Register( const EventInfo &info );

	// send an event
	void Send( Event &e );

	// hook an event
	EventHookPtr Hook( const EventInfo &info, EventHandler &handler );
};

}