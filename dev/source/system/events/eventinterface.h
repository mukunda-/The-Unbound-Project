//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2015, Mukunda Johnson, All rights reserved. =========//

#pragma once

#include "system/forwards.h"
#include "event.h"

//-----------------------------------------------------------------------------
namespace System {

//-----------------------------------------------------------------------------
class EventInterface final {

	using EventDataPtr = std::unique_ptr<EventData>;
	std::unordered_map< std::string, EventDataPtr > m_map;

	
public:
	EventInterface();
	~EventInterface();

	void Register( const EventInfo &info );

	void Send( Event &e );
	EventHookPtr Hook( const EventInfo &info, Event::Handler handler );
	
};

}