//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2015, Mukunda Johnson, All rights reserved. =========//

#include "stdafx.h"
#include "event.h"
#include "eventdata.h"

//-----------------------------------------------------------------------------
namespace System {

//-----------------------------------------------------------------------------
EventData::EventData( const EventInfo &info ) : m_info( info ) {
	
}

EventData::~EventData() {}

//-----------------------------------------------------------------------------
}