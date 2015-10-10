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
	assert( m_work == 0 );
}

//-----------------------------------------------------------------------------
void Module::AddWork() {
	std::lock_guard< std::mutex > lock( m_mutex );

	m_work++;

	if( m_work == 1 ) {
		g_main->OnModuleBusy( *this );
	}
}

//-----------------------------------------------------------------------------
void Module::RemoveWork() {
	
	std::lock_guard< std::mutex > lock( m_mutex );

	assert( m_work > 0 );

	m_work--;

	if( m_work == 0 ) {
		g_main->OnModuleIdle( *this );
	}
}

//-----------------------------------------------------------------------------
bool Module::Busy() {
	std::lock_guard< std::mutex > lock( m_mutex );
	return m_work != 0;
}

/*
//-----------------------------------------------------------------------------
void Module::SetBusy( bool busy ) {
	if( m_busy == busy ) return;

	m_busy = busy;
	if( !m_busy ) {
		g_main->OnModuleIdle( *this );
	} else {
		g_main->OnModuleBusy( *this );
	}
}*/

//-----------------------------------------------------------------------------
void Module::OnFrame() {}
void Module::OnLoad() {}
void Module::OnPrepare() {}
void Module::OnStart() {}
void Module::OnShutdown() {}
void Module::OnUnload() {}


//-----------------------------------------------------------------------------
}
 