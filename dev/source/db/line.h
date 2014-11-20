//===========================  The Unbound Project  =========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

#pragma once

#include "endpoint.h"

//-----------------------------------------------------------------------------
namespace DB {
	 
	class Line {
		std::unique_ptr<sql::Connection> m_connection;

	public:
		Line( Manager &manager, const Endpoint &endpoint );

		std::unique_ptr<sql::Statement> CreateStatement() { 
			return std::unique_ptr<sql::Statement>(m_connection->createStatement());
		}

		sql::Connection *operator->() { 
			return m_connection.get(); 
		}
	};

}
