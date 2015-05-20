//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2015, Mukunda Johnson, All rights reserved. =========//

#include "stdafx.h"
#include "eventhook.h"
#include "system/system.h"

namespace System {

	extern Main *g_main;

	//-------------------------------------------------------------------------
	EventHook::EventHook( int id ) : m_id(id) {}

	//-------------------------------------------------------------------------
	EventHook::~EventHook() {
		g_main->UnhookEvent( id );
	}
}