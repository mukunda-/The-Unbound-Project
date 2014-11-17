//===========================  The Unbound Project  =========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

#pragma once

//-----------------------------------------------------------------------------
namespace DB {

//-----------------------------------------------------------------------------
class Connection {
	// connection pool, one per thread
	std::list< std::unique_ptr<sql::Connection> > m_conpool;

public:

	/// -----------------------------------------------------------------------
	/// Create a new sql connection.
	///
	/// @param endpoint Connection and database information.
	/// @param threads How many threads this connection can use at a time.
	///
	Connection( const Endpoint &endpoint, int threads );
};

}
