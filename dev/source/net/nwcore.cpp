//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

#include "stdafx.h"
#include "nwcore.h"
#include "system/console.h"
#include "system/system.h"

//-------------------------------------------------------------------------------------------------
namespace Net {

Instance *g_instance;

//-------------------------------------------------------------------------------------------------
Instance::Instance( int threads ) {
	
	g_instance = this;
	if( threads > 0 ) {
		m_service.Run( threads );
	}
}

//-------------------------------------------------------------------------------------------------
Instance::~Instance() {
	m_service.Finish( true );
	g_instance = nullptr;  
}

System::Service &Instance::GetService() {
	return m_service;
}

//-------------------------------------------------------------------------------------------------
System::Service &DefaultService() {
	assert(g_instance);
	return g_instance->GetService();
}

//-------------------------------------------------------------------------------------------------
}
