//===========================  The Unbound Project  =========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

#include <stdafx.h>
#include "core.h"
#include "connection.h"
#include "system/console.h"

//-----------------------------------------------------------------------------
namespace DB {

Instance *g_instance = nullptr;

//-----------------------------------------------------------------------------
void Register( ConnectionPtr &&connection ) {
	g_instance->RegisterConnection( std::move(connection) );
} 

//-----------------------------------------------------------------------------
void Instance::RegisterConnection( ConnectionPtr &&connection ) {
	if( m_conmap.count( connection->Name() ) ) {
		throw std::invalid_argument( 
				"Connection name is already registered." );
	}

	//m_connections.push_back( std::move(connection) );
	m_conmap[connection->Name()] = std::move(connection);
}

//-----------------------------------------------------------------------------
void Instance::ThreadMain() {
	std::unique_lock<std::mutex> lock( m_work_mutex );
	while( 1 ){ // wait for work ) {
		m_work_signal.wait( lock );
	}
}

//-----------------------------------------------------------------------------
Instance::Instance( int threads ) {
	g_instance = this;

	for( int i = 0; i < threads; i++ ) {
		m_threadpool.push_back( 
			std::thread( std::bind( &Instance::ThreadMain, this )));
	}

	m_driver = sql::mysql::get_mysql_driver_instance(); 
	 
//	System::Console::Print( "%s", "Database subsystem started." );
}

//-----------------------------------------------------------------------------
Instance::~Instance() {
	g_instance = nullptr;
}

}
