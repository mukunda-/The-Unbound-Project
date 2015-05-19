//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2015, Mukunda Johnson, All rights reserved. =========//

#include "stdafx.h"
#include "eventlistener.h"

//-----------------------------------------------------------------------------
namespace System {

//-----------------------------------------------------------------------------
EventListener::EventListener() {}

//-----------------------------------------------------------------------------
EventListener::~EventListener() {

}

//-----------------------------------------------------------------------------
int EventListener::Hook( uint32_t code, Event::Handler handler ) {
	int hook = System::HookEvent( 
}

//-----------------------------------------------------------------------------
}