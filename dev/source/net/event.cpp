//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

#include <stdafx.h>
#include "eventhandler.h"

namespace Net {
	
//-----------------------------------------------------------------------------
namespace {
	EventInterface g_null_interface;
}

//-----------------------------------------------------------------------------
EventHandler() {
	m_disabled = false;
}

~EventHandler() {
	if( !m_disabled ) {
		throw std::runtime_error( "Destructing live event handler." );
	}
}

//-----------------------------------------------------------------------------
void EventHandler::Disable() {
	std::lock_guard<std::mutex> lock( m_mutex );
	m_disabled = true;
}

//-----------------------------------------------------------------------------
EventLock::EventLock( EventHandler &handler ) :
		m_lock( handler.m_lock ),
		m_interface( handler.m_disabled ? g_null_interface : handler ) {

}


}