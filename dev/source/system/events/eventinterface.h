//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2015, Mukunda Johnson, All rights reserved. =========//

#pragma once

#include "system/forwards.h"

//-----------------------------------------------------------------------------
namespace System {

//-----------------------------------------------------------------------------
class EventInterface final {

	using EventDataPtr = std::shared_ptr<EventData>;
	
	std::unordered_map< int,         EventDataPtr > m_code_map;
	std::unordered_map< std::string, EventDataPtr > m_name_map;

	EventData* GetEventData( const Event &e );
	
public:
	EventInterface();
	~EventInterface();

	void Send( Event &e );
};

}