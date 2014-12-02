//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

#pragma once

#include "line.h"

namespace DB {

	/// -----------------------------------------------------------------------
	/// sql::Statement wrapper with some utilities.
	///
	class Statement {
		std::unique_ptr<sql::Statement> m_base;
		Line &m_parent;
		bool m_needsflush = false;

		std::unique_ptr<sql::ResultSet> GetResultSet();

	public:
		Statement( Line &line ) : m_parent(line) {
			m_base = std::unique_ptr<sql::Statement>( 
							line->createStatement() );
		}
		virtual ~Statement() {
			FlushResultSets();
		}

		/// -------------------------------------------------------------------
		/// Access to the sql statement object.
		///
		sql::Statement *operator->() {
			return m_base.get();
		}
		
		/// -------------------------------------------------------------------
		/// Wrapper for Statement->executeQuery with templating.
		///
		/// @param statement Query template.
		/// @param args      Format arguments.
		/// @returns result from sql::Statement::executeQuery.
		///
		template < typename ... Args >
		std::unique_ptr<sql::ResultSet> ExecuteQuery( 
				const std::string &statement,
				Args ... args ) {
			FlushResultSets();

			return std::unique_ptr<sql::ResultSet>( m_base->executeQuery( 
					m_parent.BuildQueryEx( statement, args... )));
		}
		
		/// -------------------------------------------------------------------
		/// Wrapper for Statement->executeUpdate with templating.
		///
		/// @param statement Query template.
		/// @param args      Format arguments.
		/// @returns result from sql::Statement::executeUpdate.
		///
		template < typename ... Args >
		int ExecuteUpdate( const std::string &statement, Args ... args ) {
			FlushResultSets();
			return m_base->executeUpdate( 
					m_parent.BuildQueryEx( statement, args... ));
		}
		
		/// -------------------------------------------------------------------
		/// Wrapper for Statement->execute with templating.
		///
		/// @param statement Query template.
		/// @param args      Format arguments.
		/// @returns first result set.
		///
		template < typename ... Args >
		std::unique_ptr<sql::ResultSet> Execute( const std::string &statement, Args ... args ) {
			m_base->execute( m_parent.BuildQueryEx( statement, args... ));
			m_needsflush = true;
			return GetResultSet();
		}

		/// -------------------------------------------------------------------
		/// Get the next result set from a multi query.
		///
		/// @param expected "result set is expected" If true, then if there 
		///        are no more results, the transaction will be terminated
		///        by an exception.
		/// @returns Pointer to result set or nullptr if there are no more
		///          results.
		///
		std::unique_ptr<sql::ResultSet> GetNextResultSet( 
				bool expected = true );
		
		/// -------------------------------------------------------------------
		/// Flush any waiting result sets.
		///
		void FlushResultSets();

	};
}