//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2015, Mukunda Johnson, All rights reserved. =========//

#pragma once

#include "system/system.h"
#include "system/module.h"

//-----------------------------------------------------------------------------
namespace System {

extern Main *g_main;

//------------------------------------------------------------------------------
Module::Module( Levels level ) {
	m_level = level;

}

//------------------------------------------------------------------------------
void Module::SetBusy( bool busy ) {
	if( m_busy == busy ) return;

	m_busy = busy;
	if( !m_busy ) {
		g_main->OnModuleIdle( *this );
	}
}

//-----------------------------------------------------------------------------
}
