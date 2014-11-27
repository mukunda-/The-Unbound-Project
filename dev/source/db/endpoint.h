//===========================  The Unbound Project  =========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

#pragma once

namespace DB {

/// ---------------------------------------------------------------------------
/// Description for a connection.
///
struct Endpoint {

	// remote address of database
	// e.g. "example.com:3306"
	// port number is optional
	std::string address;

	// username to log in with
	std::string username;

	// password to log in with
	std::string password;

	// database to access 
	// leave empty to not associate queries with a single database.
	std::string database;

	Endpoint() {}

	Endpoint( const std::string &p_address, const std::string &p_username,
			  const std::string &p_password, const std::string &p_database ) :
				address( p_address ), username( p_username ),
				password( p_password ), database( p_database )
	{
	}
};

}