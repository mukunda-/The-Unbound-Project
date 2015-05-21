//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2015, Mukunda Johnson, All rights reserved. =========//

#include "stdafx.h"
#include "event.h"
#include "eventdata.h"
#include "eventhook.h"

//-----------------------------------------------------------------------------
namespace System {

//-----------------------------------------------------------------------------
EventData::EventData( const EventInfo &info ) : m_info( info ) {}

//-----------------------------------------------------------------------------
EventData::~EventData() {}

//-----------------------------------------------------------------------------
void EventData::AddHook( EventHookPtr &hook ) {

	m_hooks.push_back( hook );
}

//-----------------------------------------------------------------------------
void EventData::Accept( Event &e ) {

	// send to all hooks and erase ones that were deleted

	for( auto &hook : m_hooks ) {
		auto locked = hook.lock();
		if( locked ) {
			(*locked)( e );
		}
	}

	// remove deleted hooks
	m_hooks.erase( std::remove_if( m_hooks.begin(), m_hooks.end(), 
		[]( const WeakHook& hook ) {
			return hook.expired();
		}),
		m_hooks.end());
}

//-----------------------------------------------------------------------------
}