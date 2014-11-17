//===========================  The Unbound Project  =========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

#include <stdafx.h>
#include "core.h"
#include "system/console.h"

//-----------------------------------------------------------------------------
namespace DB {

Instance *g_instance = nullptr;

//-----------------------------------------------------------------------------
void Register( const std::string &name, const Endpoint &info ) {
	g_instance->RegisterConnection( name, info );
} 

//-----------------------------------------------------------------------------
void Instance::RegisterConnection( const std::string &name, 
								   const Endpoint &info ) {

}

//-----------------------------------------------------------------------------
Instance::Instance( int threads ) {
	g_instance = this;

	for( int i = 0; i < threads; i++ ) {
		m_threadpool.push_back( std::thread( std::bind( 
	}

	m_driver = sql::mysql::get_mysql_driver_instance(); 

	
	System::Console::Print( "%s", "Database subsystem started." );
}

//-----------------------------------------------------------------------------
Instance::~Instance() {
	g_instance = nullptr;
}

}
