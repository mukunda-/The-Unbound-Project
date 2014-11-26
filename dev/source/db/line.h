//===========================  The Unbound Project  =========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

#pragma once

#include "forwards.h"
#include "endpoint.h"
#include "querybuilder.h"

//-----------------------------------------------------------------------------
namespace DB {
	class Statement;
	class Line {
		std::unique_ptr<sql::Connection> m_connection; 

	public:
		Line( Manager &manager, const Endpoint &endpoint );
		virtual ~Line() {}
		 
		std::unique_ptr<Statement> CreateStatement();

		sql::Connection *operator->() { 
			return m_connection.get(); 
		}

		sql::Connection &GetConnection() {
			return *m_connection;
		}

		/// -------------------------------------------------------------------
		/// Build a query.
		///
		/// @param format Template for query.
		/// @param args   Formatted arguments.
		/// @returns QueryBuilder object with the args present fed to it.
		///
		template < typename ... Args >
		QueryBuilder BuildQuery( const std::string &format, Args ... args ) {
			QueryBuilder builder( *this, format );
			Util::Feed( builder, args... );
			return builder; 
		}
		
		/// -------------------------------------------------------------------
		/// Build a query faster.
		///
		/// @param format Template for query.
		/// @param args   Formatted arguments.
		/// @returns SQLString ready for execution. If the template isn't
		///          satisfied an exception will be thrown.
		///
		template < typename ... Args >
		sql::SQLString BuildQueryEx( const std::string &format, Args ... args ) {
			if( sizeof...(args) == 0 ) {
				return format.c_str();
			} else {
				QueryBuilder builder( *this, format );
				Util::Feed( builder, args... );
				return builder.SQLString(); 
			}
		}

	};

}
