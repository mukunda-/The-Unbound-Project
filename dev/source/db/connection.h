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
	friend class Manager;

	Manager     &m_manager;
	std::string m_name;
	Endpoint    m_endpoint;
	
	std::mutex  m_mut;
	int         m_free_threads;
	std::deque<TransactionPtr> m_work_queue;
	
	std::stack<LinePtr> m_linepool;
	// connection pool, one per thread
	//Util::SLinkedList<Connection> m_conpool;

	// this is called from a work thread.
	void DoTransaction( TransactionPtr &t );

	// get a line from the linepool or create one.
	LinePtr GetLine();

	// push a line back into the linepool
	void PushLine( LinePtr &&line, bool thread_freed = true );

	// increment the free thread counter.
	void FreeThread();

public:

	/// -----------------------------------------------------------------------
	/// Create a new database connection.
	///
	/// @param manager  DB manager instance.
	/// @param name     Name to identify this connection.
	/// @param endpoint Connection and database information.
	/// @param threads  How many threads this connection can use at a time.
	///
	Connection( Manager &manager, const std::string &name, 
				const Endpoint &endpoint, int threads = 1 );
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
	void Execute( TransactionPtr &t );
};

}
