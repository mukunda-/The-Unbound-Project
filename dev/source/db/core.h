//===========================  The Unbound Project  =========================//
//                                                                           //
//========= Copyright � 2014, Mukunda Johnson, All rights reserved. =========//

#pragma once

#include "connection.h"

//-----------------------------------------------------------------------------
namespace DB {

//-----------------------------------------------------------------------------
class Instance {
	std::vector<std::unique_ptr<Connection>> m_connections;
	sql::mysql::MySQL_Driver *m_driver;

public:
	Instance();
	~Instance();
};

}
