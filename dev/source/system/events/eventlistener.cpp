//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2015, Mukunda Johnson, All rights reserved. =========//

#include "stdafx.h"

#if 0
#include "eventlistener.h"

//-----------------------------------------------------------------------------
namespace System {

//-----------------------------------------------------------------------------
EventListener::EventListener() {}

//-----------------------------------------------------------------------------
EventListener::~EventListener() {
	for( auto i : m_hook_ids ) {
		System::UnhookEvent( i );
	}
}

//-----------------------------------------------------------------------------
int EventListener::Hook( uint32_t code, Event::Handler handler ) {
	int hook = System::HookEvent( 
}

//-----------------------------------------------------------------------------
void EventListener::Unhook( int hookid ) {
	for( auto i = m_hook_ids.begin(); i != m_hook_ids.end(); i++ ) {
		if( *i == hookid ) {
			int id = *i;
			m_hook_ids.erase(i);

			System::UnhookEvent( id );
		}
	}
}

//-----------------------------------------------------------------------------
}
#endif
