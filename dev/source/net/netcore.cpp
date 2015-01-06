//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

#include "stdafx.h"
#include "core.h"
#include "system/console.h"
#include "system/system.h"

//-----------------------------------------------------------------------------
namespace Net {

namespace {
	Instance *g_instance;
}

//-----------------------------------------------------------------------------
Stream::ptr Connect( const std::string &host, const std::string &service, 
					 StreamFactory factory ) {

	Stream::ptr stream = factory();
	stream->Connect( host, service );
	return stream;
}

//-----------------------------------------------------------------------------
void ConnectAsync( const std::string &host, const std::string &service, 
				   StreamFactory factory ) {

	Stream::ptr stream = factory(); 
	stream->ConnectAsync( host, service );
}

//-----------------------------------------------------------------------------
Instance::Instance( int threads ) {
	
	g_instance = this;
//	if( threads > 0 ) {
//		m_service.Run( threads );
//	}
}

//-----------------------------------------------------------------------------
Instance::~Instance() {
	//m_service.Finish( true );
	g_instance = nullptr;  
}

//-----------------------------------------------------------------------------
System::Service &Instance::GetService() {
	return System::GetService();
	//return m_service;
}

//-----------------------------------------------------------------------------
System::Service &DefaultService() {
	assert(g_instance);
	return g_instance->GetService();
}

//-----------------------------------------------------------------------------
}
