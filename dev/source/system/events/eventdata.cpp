//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2015, Mukunda Johnson, All rights reserved. =========//

#include "stdafx.h"
#include "event.h"
#include "eventdata.h"

//-----------------------------------------------------------------------------
namespace System {

//-----------------------------------------------------------------------------
EventData::EventData( const EventInfo &info ) : m_info( info ) {}

//-----------------------------------------------------------------------------
EventData::~EventData() {}

//-----------------------------------------------------------------------------
int EventData::AddHandler( EventHandler &handler ) {

	m_handlers.push_back( handler );
	return m_next_id++;
}

//-----------------------------------------------------------------------------
}