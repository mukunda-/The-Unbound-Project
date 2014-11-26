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
	public:
		Statement( Line &line ) : m_parent(line) {
			m_base = std::unique_ptr<sql::Statement>( 
							line->createStatement() );
		}
		virtual ~Statement() {}

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
			return m_base->executeUpdate( 
					m_parent.BuildQueryEx( statement, args... ));
		}
		
		/// -------------------------------------------------------------------
		/// Wrapper for Statement->execute with templating.
		///
		/// @param statement Query template.
		/// @param args      Format arguments.
		/// @returns result from sql::Statement::execute.
		///
		template < typename ... Args >
		bool Execute( const std::string &statement, Args ... args ) {
			return m_base->executeUpdate( 
					m_parent.BuildQueryEx( statement, args... ));
		}

	};
}