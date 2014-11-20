//===========================  The Unbound Project  =========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

#pragma once

// MySQL Connector/C++ Interface

#include "connection.h"
#include "forwards.h"
#include "util/trie.h"
#include "util/stringmap.h"

//-----------------------------------------------------------------------------
namespace DB {

/// ---------------------------------------------------------------------------
/// Register a database connection
///
/// @param connection Connection to register. The subsystem will take
///        ownership of the pointer.
///
Connection &Register( const std::string &name, const Endpoint &endpoint );

//-----------------------------------------------------------------------------
class Manager {
	
public:
	/// -----------------------------------------------------------------------
	/// Start the database subsystem
	///
	/// @param threads How many threads to create for the database
	///        thread pool.
	///
	Manager( int threads );
	~Manager();

	//-------------------------------------------------------------------------
private:
	std::unordered_map< std::string, ConnectionPtr > m_conmap;

	sql::mysql::MySQL_Driver &m_driver;
	std::vector<std::thread> m_threadpool;

	std::deque<std::unique_ptr<Transaction>> m_work_queue; // todo, use arena allocator.
	bool m_shutdown = false;
	std::mutex m_work_mutex;
	std::condition_variable m_work_signal;

	void ThreadMain();
	void ExecuteTransaction( TransactionPtr transaction );

	//-------------------------------------------------------------------------
public: // wrapped by global functions.
	Connection &RegisterConnection( const std::string &name, 
									const Endpoint &endpoint );

	//-------------------------------------------------------------------------
private:
	friend class Line;
	friend class Connection;

	// called by Line:
	std::unique_ptr<sql::Connection> Connect( const Endpoint &endpoint );

	// called by Connection:
	void QueueWork( std::unique_ptr<Transaction> &&work );
	
};

}
