//===========================  The Unbound Project  =========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

#pragma once

#include "util/slinkedlist.h"
#include "transaction.h"
#include "forwards.h"
#include "endpoint.h"
#include "line.h"

//-----------------------------------------------------------------------------
namespace DB {

//-----------------------------------------------------------------------------
class Connection : public Util::SLinkedItem<Connection> {
	
	std::string m_name;
	Endpoint    m_endpoint;
	
	std::mutex  m_mut;
	int         m_free_threads;
	std::deque<std::unique_ptr<Transaction>> m_work_queue;
	
	std::stack<std::unique_ptr<Line>> m_stack;
	// connection pool, one per thread
	//Util::SLinkedList<Connection> m_conpool;

	// this is called from a work thread.
	void DoTransaction( Transaction &t );

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
	~Connection();
	
	/// -----------------------------------------------------------------------
	/// @returns the name of this connection.
	///
	const std::string &Name() { return m_name; }
	
	/// -----------------------------------------------------------------------
	/// Execute a transaction.
	///
	/// @param t Transaction; ownership is given to the connection and is
	///          no longer accessible outside.
	///
	void Execute( std::unique_ptr<Transaction> &t );
};

}
