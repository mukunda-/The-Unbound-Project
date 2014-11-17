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
/// @param name Name to associate with connection.
/// @param info Database connection information.
/// @throws std::runtime_error if the connection already exists
///
void Register( const std::string &name, const Endpoint &info );

//-----------------------------------------------------------------------------
class Instance {
	std::vector<std::unique_ptr<Connection>> m_connections;
	Util::Trie<Connection*> m_conmap;

	sql::mysql::MySQL_Driver *m_driver;

	std::vector<std::thread> m_threadpool;

	std::list< Transaction > m_pending_xs;

public:
	/// -----------------------------------------------------------------------
	/// Start the database subsystem
	///
	/// @param threads How many threads to create for the database
	///        thread pool.
	///
	Instance( int threads );
	~Instance();

	void RegisterConnection( const std::string &name, const Endpoint &info );
};

}
