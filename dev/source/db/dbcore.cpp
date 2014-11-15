//===========================  The Unbound Project  =========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

#include <stdafx.h>
#include "core.h"

//-----------------------------------------------------------------------------
namespace DB {

Instance *g_instance;

//-----------------------------------------------------------------------------
Instance::Instance() {
	g_instance = this;
	m_driver = sql::mysql::get_mysql_driver_instance();

}

//-----------------------------------------------------------------------------
Instance::~Instance() {
	g_instance = nullptr;
}

}
