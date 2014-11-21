//===========================  The Unbound Project  =========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

#include <stdafx.h>
#include "querybuilder.h"
#include "line.h"

namespace DB {

//-----------------------------------------------------------------------------
QueryBuilder::QueryBuilder( Line &line, const std::string &format ) 
		: m_conn(line.GetConnection()), m_formatter(format) {

}

//-----------------------------------------------------------------------------
QueryBuilder::QueryBuilder( sql::Connection &connection, 
							const std::string &format ) 
		: m_conn(connection), m_formatter(format) {

}

//-----------------------------------------------------------------------------
QueryBuilder &QueryBuilder::FeedEscapedString( const std::string &arg ) {
	sql::mysql::MySQL_Connection& mysql_conn = 
			dynamic_cast<sql::mysql::MySQL_Connection&>(m_conn);

	std::string escaped = mysql_conn.escapeString( arg.c_str() );
	escaped = "'" + escaped + "'";
	m_formatter % escaped;
	return *this;
}

//-----------------------------------------------------------------------------
QueryBuilder &QueryBuilder::FeedUnescapedString( const std::string &arg ) {
	m_formatter % arg;
	return *this;
}

}