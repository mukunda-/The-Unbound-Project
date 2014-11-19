//===========================  The Unbound Project  =========================//
//                                                                           //
//========= Copyright � 2014, Mukunda Johnson, All rights reserved. =========//

#pragma once

#include "endpoint.h"

//-----------------------------------------------------------------------------
namespace DB {
	 
	class Line {
		std::unique_ptr<sql::Connection> m_connection;

	public:
		Line( const Endpoint &endpoint );
	};

}
