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
	std::string m_address;

	// username to log in with
	std::string m_username;

	// password to log in with
	std::string m_password;

	// database to access
	std::string m_database;

	Endpoint() {}

	Endpoint( const std::string &address, const std::string &username,
			  const std::string &password, const std::string &database ) :
				m_address( address ), m_username( username ),
				m_password( password ), m_database( database )
	{
	}
};

}