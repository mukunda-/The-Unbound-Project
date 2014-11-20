//===========================  The Unbound Project  =========================//
//                                                                           //
//========= Copyright � 2014, Mukunda Johnson, All rights reserved. =========//

#pragma once

namespace DB {

	/// -----------------------------------------------------------------------
	/// A failure occurs when the database system fails and cannot recover.
	///
	class Failure : public std::runtime_error {

		// the error code when the failure was determined. 
		// (it may just be the last one after several retries.)
		int m_mysql_error;
		const std::string m_sqlstate;

	public:
		Failure( const sql::SQLException &e ) :
				m_mysql_error( e.getErrorCode() ),
				m_sqlstate( e.getSQLState() ),
				std::runtime_error( e.what() )
		{
		}
		Failure( int mysql_error, const std::string &sqlstate, const std::string &message ) :
				m_mysql_error( mysql_error ),
				m_sqlstate( sqlstate ),
				std::runtime_error( message )
		{
		}

		/// -------------------------------------------------------------------
		/// @returns the MySQL specific error code for the failure.
		///
		int MySQLCode() const { return m_mysql_error; }

		/// -------------------------------------------------------------------
		/// @returns the standardized SQL state string for the failure.
		///
		const std::string &SQLState() const { return m_sqlstate; }
	};
}