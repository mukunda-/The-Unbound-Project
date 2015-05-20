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
void EventInterface::Register( const EventInfo &info ) {
	if( m_map.count( info.name ) != 0 ) {
		throw std::runtime_error( "Duplicate event registration." );
	}

	m_map[ info.name ] = EventDataPtr( new EventData( info ));
}

//-----------------------------------------------------------------------------
void EventInterface::Send( Event &e ) {
	
	assert( m_map.count( e.Name() ) != 0 );
	
	auto data = m_map.at( e.Name() ).get();

	data->Accept( e );
}

}