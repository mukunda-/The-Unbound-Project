//===========================  The Unbound Project  =========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

#pragma once

#include "forwards.h"
#include "endpoint.h"

//-----------------------------------------------------------------------------
namespace DB {

//-----------------------------------------------------------------------------
class Connection {

	std::string m_name;
	Endpoint    m_endpoint;

	// connection pool, one per thread
	std::list< std::unique_ptr<sql::Connection> > m_conpool;

public:

	/// -----------------------------------------------------------------------
	/// Create a new sql connection.
	///
	/// @param name     Name to identify this connection.
	/// @param endpoint Connection and database information.
	/// @param threads  How many threads this connection can use at a time.
	///
	Connection( const std::string &name, const Endpoint &endpoint, 
				int threads = 1 );

	const std::string &Name() { return m_name; }
};

}
