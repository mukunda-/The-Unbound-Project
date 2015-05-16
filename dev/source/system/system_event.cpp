//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2015, Mukunda Johnson, All rights reserved. =========//

#include "event.h"
#include "system.h"

//-----------------------------------------------------------------------------
namespace System {

extern Main *g_main;

//-----------------------------------------------------------------------------
Event::Event( const std::string &name ) : m_name(name) {
	m_id = GetEventID( name );

}

}
