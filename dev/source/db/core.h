//===========================  The Unbound Project  =========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

#pragma once

// MySQL Connector/C++ Interface

#include "connection.h"
#include "forwards.h"
#include "util/trie.h"

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

private:
	std::vector<ConnectionPtr> m_connections;
	Util::Trie<Connection*> m_conmap;

	sql::mysql::MySQL_Driver *m_driver;
	std::vector<std::thread> m_threadpool;
//	std::list< Transaction > m_pending_xs;

public: // wrapped by global functions.
	void RegisterConnection( ConnectionPtr &&connection );
};

}
