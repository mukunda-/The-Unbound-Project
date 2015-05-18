//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2015, Mukunda Johnson, All rights reserved. =========//

#pragma once

namespace System {

class EventInterface final {
	
	std::unordered_map< int,         EventData& > m_code_map;
	std::unordered_map< std::string, EventData& > m_name_map;
	std::vector< std::unique_ptr<EventData> >     m_event_data;

public:
	EventInterface();

	void RegisterEvent( const EventInfo &info );	
};

}