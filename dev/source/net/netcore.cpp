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
Instance::Instance() {
	
	g_instance = this;

	// setup crypto threading
	m_crypto_locks = std::unique_ptr<std::mutex[]>( 
					new std::mutex[CRYPTO_num_locks()] );

	CRYPTO_set_locking_callback( LockingFunction );

//	if( threads > 0 ) {
//		m_service.Run( threads );
//	}
}

//-----------------------------------------------------------------------------
Instance::~Instance() {
	//m_service.Finish( true );

	CRYPTO_set_locking_callback( NULL );

	g_instance = nullptr;  
}

/// ---------------------------------------------------------------------------
/// Callback for OpenSSL thread safety.
///
void Instance::LockingFunction( int mode, int n, const char *, int ) {

	if( mode & CRYPTO_LOCK ) {
		g_instance->m_crypto_locks[n].lock();
	} else {
		g_instance->m_crypto_locks[n].unlock();
	}
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
