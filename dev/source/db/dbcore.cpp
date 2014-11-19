//===========================  The Unbound Project  =========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

#include <stdafx.h>
#include "core.h"
#include "connection.h"
#include "system/console.h"

using namespace std;

//-----------------------------------------------------------------------------
namespace DB {

Instance *g_instance = nullptr;

//-----------------------------------------------------------------------------
void Register( const string &name, const Endpoint &endpoint ) {
	g_instance->RegisterConnection( name, endpoint );
}

//-----------------------------------------------------------------------------
void Instance::QueueWork( unique_ptr<Transaction> &&transaction ) {
	{
		lock_guard<mutex> lock(m_work_mutex);
		m_work_queue.push_back( std::move(transaction) );
	}
	m_work_signal.notify_one();
}

//-----------------------------------------------------------------------------
void Instance::RegisterConnection( const string &name, 
								   const Endpoint &endpoint ) {
	if( m_conmap.count( name ) ) {
		throw invalid_argument( 
				"Connection name is already registered." );
	}
	 
	m_conmap[ name ] = ConnectionPtr( 
			new Connection( name, endpoint ));
}

//-----------------------------------------------------------------------------
void Instance::ThreadMain() {
	unique_lock<mutex> lock( m_work_mutex, std::defer_lock );
	while( true ) {
		lock.lock();
		while( m_work_queue.empty() && !m_shutdown ) {
			m_work_signal.wait( lock );
		}
	
		if( !m_work_queue.empty() ) {
			std::unique_ptr<Transaction> transaction = 
					std::move(m_work_queue.front());
			m_work_queue.pop_front();
			lock.unlock();

			// <process>.
			continue;
		}

		if( m_shutdown ) return;
	}
}

/// ---------------------------------------------------------------------------
/// Create an sql connection. This should be done in a work thread.
///
/// @param endpoint Address and credentials to use.
///
unique_ptr<sql::Connection> Instance::Connect( const Endpoint &endpoint ) {
	return unique_ptr<sql::Connection>(
		m_driver.connect( endpoint.m_address, 
						  endpoint.m_username, 
						  endpoint.m_password ));
}

//-----------------------------------------------------------------------------
Instance::Instance( int threads ) : 
			m_driver( *sql::mysql::get_mysql_driver_instance() ) {
	g_instance = this;

	for( int i = 0; i < threads; i++ ) {
		m_threadpool.push_back( 
			std::thread( std::bind( &Instance::ThreadMain, this )));
	}
	 
	// system.console isn't available yet!
	//	System::Console::Print( "%s", "Database subsystem started." );
}

//-----------------------------------------------------------------------------
Instance::~Instance() {

	// set shutdown flag, send signal, and wait for work to finish.
	{
		lock_guard<mutex> lock(m_work_mutex);
		m_shutdown = true;
	}

	m_work_signal.notify_all();
	for( auto &i : m_threadpool ) i.join();

	g_instance = nullptr;
}

}
