//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2015, Mukunda Johnson, All rights reserved. =========//

#include "stdafx.h"
#include "event.h"
#include "eventdata.h"
#include "eventinterface.h"

//-----------------------------------------------------------------------------
namespace System {

//-----------------------------------------------------------------------------
EventInterface::EventInterface() {
}

//-----------------------------------------------------------------------------
EventInterface::~EventInterface() {
	
}

//-----------------------------------------------------------------------------
EventData* EventInterface::GetEventData( const Event &e ) {

	try {
		if( e.Code() != 0 ) {
			return m_code_map.at( e.Code() ).get();
		} else {
			return m_name_map.at( e.Code() ).get();
		}

	} catch( std::out_of_range & ) {

		ptr = std::make_shared<EventData>( e.INFO() );

		if( code != 0 ) {
			m_code_map[code] = 
	}
}

//-----------------------------------------------------------------------------
void EventInterface::Send( Event &e ) {

	EventDataPtr ed = GetEventData( ;

	if( e.code != 0 ) {
		ed = GetEventData( e.Code() );
	} else {
		ed = GetEventData( e.Name() );
	}
}

}