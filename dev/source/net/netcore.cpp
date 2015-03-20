//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

#include "stdafx.h"
#include "core.h"
#include "system/console.h"
#include "system/system.h"

// we don't need to be thread safe, since we are doing ALL ssl operations
// with a single strand
//#define SETUP_CRYPTO_LOCKS

//-----------------------------------------------------------------------------
namespace Net {

Instance *g_instance;

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
Instance::Instance() : m_ssl_strand( System::GetService()() ), 
		Module( System::Module::Levels::SUBSYSTEM ) {
	
	Module::SetName( "net" );

	g_instance = this;

#ifdef SETUP_CRYPTO_LOCKS
	// setup crypto threading
	//m_crypto_locks = std::unique_ptr<std::mutex[]>( 
	//				new std::mutex[CRYPTO_num_locks()] );

	//CRYPTO_set_locking_callback( LockingFunction );
#endif
	 
}

//-----------------------------------------------------------------------------
Instance::~Instance() {
	//m_service.Finish( true );
	std::lock_guard<std::mutex> lock(m_lock);
	assert( m_work_counter == 0 ); // should not be destructed otherwise.

	//WaitUntilWorkIsFinished();

#ifdef SETUP_CRYPTO_LOCKS
	//CRYPTO_set_locking_callback( NULL );
#endif

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
void Instance::AddWork() {
	std::lock_guard<std::mutex> lock(m_lock);

	m_work_counter++; 

	SetBusy( true );
}

//-----------------------------------------------------------------------------
void Instance::RemoveWork() {
	std::lock_guard<std::mutex> lock(m_lock);

	m_work_counter--; 

	if( m_work_counter == 0 ) {
		SetBusy(false);
	}
}

//-----------------------------------------------------------------------------
System::Service &Instance::GetService() {
	// we just use the main system service now.
	return System::GetService(); 
}

//-----------------------------------------------------------------------------
System::Service &DefaultService() {
	assert(g_instance);
	return g_instance->GetService();
}

//-----------------------------------------------------------------------------
}
