//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

#include "stdafx.h"
#include "network/nwcore.h"
#include "system/console.h"
#include "system/system.h"

//-------------------------------------------------------------------------------------------------
namespace Network {

System::Service *g_default_service = nullptr;
//Service *g_default_service = nullptr;


//-------------------------------------------------------------------------------------------------
Init::Init( int threads ) {
	assert( g_default_service == nullptr );
	g_default_service = new System::Service;

	if( threads > 0 )
		g_default_service->Run( threads );
}

//-------------------------------------------------------------------------------------------------
Init::~Init() {
	assert( g_default_service );
	g_default_service->Finish();
	delete g_default_service;
	g_default_service = nullptr;
}

//-------------------------------------------------------------------------------------------------
System::Service &DefaultService() {
	assert( g_default_service );
	return *g_default_service;
}

//-------------------------------------------------------------------------------------------------
}
