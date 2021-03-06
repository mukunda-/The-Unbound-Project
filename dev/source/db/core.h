//===========================  The Unbound Project  =========================//
//                                                                           //
//========= Copyright � 2014, Mukunda Johnson, All rights reserved. =========//

#pragma once

// MySQL Connector/C++ Interface

#include "connection.h"
#include "forwards.h"
#include "util/trie.h"
#include "util/stringmap.h"

#include "system/module.h"

//-----------------------------------------------------------------------------
namespace DB {

/// ---------------------------------------------------------------------------
/// Register a database connection
///
/// @param name     Name to assign to connection.
/// @param endpoint DB endpoint information.
/// @param threads  How many threads the connection can use at once.
///
/// @returns Reference to newly created connection.
///
Connection &Register( const std::string &name, const Endpoint &endpoint, 
					  int threads = 1 );

/// ---------------------------------------------------------------------------
/// Get a database connection by name.
///
/// @param name Name the connection was registered with.
/// @returns    Database connection.
///
Connection &Get( const std::string &name );

//-----------------------------------------------------------------------------
class Manager final : public System::Module {
	
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

	std::deque<TransactionPtr> m_work_queue; // todo, use arena allocator.
	bool m_shutdown = false;
	std::condition_variable m_work_signal;

	std::mutex m_mutex;

	int m_work_counter = 0; 

	void WorkAdded();
	void WorkCompleted();

	void ThreadMain();
	void DoActions( TransactionPtr &transaction, LinePtr &line, int &retries );
	void ExecuteTransaction( TransactionPtr transaction );

	friend class Work;

	//-------------------------------------------------------------------------
public: // wrapped by global functions.
	Connection &RegisterConnection( const std::string &name, 
									const Endpoint &endpoint, int threads );
	Connection &GetConnection( const std::string &name );

	static sql::SQLString SqlString( const std::string &source );

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
