//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2015, Mukunda Johnson, All rights reserved. =========//

#include <stdafx.h>
#include "system/system.h"
#include "system/module.h"

//-----------------------------------------------------------------------------
namespace System {

extern Main *g_main;

//-----------------------------------------------------------------------------
Module::Module( const Stref &name, Levels level ) {
	// todo, set busy when registered.
	m_name = name;
	m_level = level;

}

//-----------------------------------------------------------------------------
Module::~Module() {
	assert( !m_busy );
}

//-----------------------------------------------------------------------------
void Module::SetBusy( bool busy ) {
	if( m_busy == busy ) return;

	m_busy = busy;
	if( !m_busy ) {
		g_main->OnModuleIdle( *this );
	} else {
		g_main->OnModuleBusy( *this );
	}
}

//-----------------------------------------------------------------------------
void Module::OnLoad() {}
void Module::OnStart() {}
void Module::OnShutdown() {}
void Module::OnUnload() {}


//-----------------------------------------------------------------------------
}
 