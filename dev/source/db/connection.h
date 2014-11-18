//===========================  The Unbound Project  =========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

#pragma once

#include "util/slinkedlist.h"
#include "forwards.h"
#include "endpoint.h"

//-----------------------------------------------------------------------------
namespace DB {

//-----------------------------------------------------------------------------
class Connection : public Util::SLinkedItem<Connection> {
	
	std::string m_name;
	Endpoint    m_endpoint;
	
	Util::SLinkedItem<Transaction> m_pending_xs;

	// connection pool, one per thread
//	Util::SLinkedList<Connection> m_conpool;

private:
	void ExecuteTransaction( Transaction &t );

public:

	/// -----------------------------------------------------------------------
	/// Create a new database connection.
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
