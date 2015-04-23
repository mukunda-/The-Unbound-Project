//===========================  The Unbound Project  =========================//
//                                                                           //
//========= Copyright © 2015, Mukunda Johnson, All rights reserved. =========//

#pragma once

#include "util/uniquelist.h"
#include "transaction.h"
#include "forwards.h"
#include "endpoint.h"
#include "line.h"

//-----------------------------------------------------------------------------
namespace DB {

//-----------------------------------------------------------------------------
class Connection {
	friend class Manager;

	Manager     &m_manager;
	std::string m_name;
	Endpoint    m_endpoint;
	
	std::mutex  m_mut;
	int         m_free_threads;
	std::deque<TransactionPtr> m_work_queue;

	std::atomic<bool> m_failure = false;
	
	std::stack<LinePtr> m_linepool;
	// connection pool, one per thread
	//Util::SLinkedList<Connection> m_conpool;

	// this is called from a work thread.
	void DoTransaction( TransactionPtr &t );

	// get a line from the linepool or create one.
	LinePtr GetLine();

	/** -----------------------------------------------------------------------
	 * Push a line back into the linepool.
	 *
	 * @param line Pointer to line. May be a null pointer to only free
	 *             a thread without pushing a (failed) line.
	 */
	void PushLine( LinePtr &&line ); 

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
	virtual ~Connection();
	
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
	void Execute( TransactionPtr &&t );
	
	/// -----------------------------------------------------------------------
	/// Disallow any further transactions from being processed.
	///
	void SetFailState() { m_failure = true; }
	bool GetFailState() { return m_failure; }
};

}
