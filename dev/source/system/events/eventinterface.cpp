//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2015, Mukunda Johnson, All rights reserved. =========//

#include "stdafx.h"
#include "event.h"
#include "eventdata.h"
#include "eventhook.h"
#include "eventinterface.h"
#include "console/console.h"

#ifdef _DEBUG
#define DEBUG_PRINT_EVENTS
#endif

//-----------------------------------------------------------------------------
namespace System {

//-----------------------------------------------------------------------------
EventInterface::EventInterface() {}

//-----------------------------------------------------------------------------
EventInterface::~EventInterface() {}
 
//-----------------------------------------------------------------------------
void EventInterface::Register( const Event::Info &info ) {
	if( m_map.count( info.name ) != 0 ) {
		throw std::runtime_error( "Duplicate event registration." );
	}

	m_map[ info.name ] = EventDataPtr( new EventData( info ));

	::Console::Print( "Event registered: %s (%d)", info.name, info.code );
}

//-----------------------------------------------------------------------------
void EventInterface::Send( Event &e ) {
	
	assert( m_map.count( e.Name() ) != 0 );
	
	auto &data = m_map.at( e.Name() ) ;

#   ifdef DEBUG_PRINT_EVENTS
		::Console::Print( "Event triggered: %s", e.Name() );
#   endif

	data->Accept( e );
	 
}

//-----------------------------------------------------------------------------
EventHookPtr EventInterface::Hook( const Event::Info &info, 
	                               Event::Handler &handler ) {
	assert( m_map.count( info.name ) != 0 );
	
	auto &data = m_map.at( info.name );

	auto hook = std::make_shared< EventHook >( handler );
	data->AddHook( hook );

	return hook;
}

}