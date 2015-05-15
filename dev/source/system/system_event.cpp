//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2015, Mukunda Johnson, All rights reserved. =========//

#include "event.h"
#include "system.h"

//-----------------------------------------------------------------------------
namespace System {

extern Main *g_main;

//-----------------------------------------------------------------------------
const std::string &Event::Name() {
	return g_main->GetEventName( m_type );
}

}
