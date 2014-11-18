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
void Register( ConnectionPtr &&connection );

//-----------------------------------------------------------------------------
class Instance {
	
public:
	/// -----------------------------------------------------------------------
	/// Start the database subsystem
	///
	/// @param threads How many threads to create for the database
	///        thread pool.
	///
	Instance( int threads );
	~Instance();

	//-------------------------------------------------------------------------
private:
	std::unordered_map<std::string, ConnectionPtr> m_conmap;

	sql::mysql::MySQL_Driver *m_driver;
	std::vector<std::thread> m_threadpool;
	Util::SLinkedItem<Transaction> m_pending_xs;
//	std::list< Transaction > m_pending_xs;

	std::mutex m_work_mutex;
	std::condition_variable m_work_signal;

	void ThreadMain();

	//-------------------------------------------------------------------------
public: // wrapped by global functions.
	void RegisterConnection( ConnectionPtr &&connection );
};

}
