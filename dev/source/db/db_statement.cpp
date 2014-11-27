//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

#include "stdafx.h"
#include "statement.h"

namespace DB {

	//-------------------------------------------------------------------------
	std::unique_ptr<sql::ResultSet> Statement::GetResultSet() {
		return std::unique_ptr<sql::ResultSet>( m_base->getResultSet() );
	}

	//-------------------------------------------------------------------------
	std::unique_ptr<sql::ResultSet> Statement::GetNextResultSet() {
		if( m_base->getMoreResults() ) {
			return std::unique_ptr<sql::ResultSet>( m_base->getResultSet() );
		} else {
			m_needsflush = false;
			return std::unique_ptr<sql::ResultSet>( nullptr );
		}
	}

	//-------------------------------------------------------------------------
	void Statement::FlushResultSets() {
		if( !m_needsflush ) return;
		while( m_base->getMoreResults() ) {
			GetResultSet();
		}
		m_needsflush = false;
	}
}