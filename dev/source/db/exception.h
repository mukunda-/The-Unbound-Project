//===========================  The Unbound Project  =========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

#pragma once

namespace DB {

	class Exception : public std::runtime_error {

	public:
		enum {
			UNKNOWN,
			RECOVERABLE,
			RETRY,
			SOFTFAIL,
			HARDFAIL
		};

		Exception( const sql::SQLException &e );
		Exception( int type, const std::string &desc );

		/// -------------------------------------------------------------------
		/// @returns the MySQL specific error code for the failure.
		///
		int MySQLCode() const { return m_mysql_error; }

		int Type() const { return m_type; }

		/// -------------------------------------------------------------------
		/// @returns the standardized SQL state string for the failure.
		///
		const std::string &SQLState() const { return m_sqlstate; }

	private:
		int m_type = UNKNOWN;
		
		// mysql specific error code.
		int m_mysql_error = 0;

		// sqlstate for sql error.
		std::string m_sqlstate;
	};
}