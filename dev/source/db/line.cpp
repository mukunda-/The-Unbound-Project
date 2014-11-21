//===========================  The Unbound Project  =========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

#include "stdafx.h"
#include "core.h"
#include "line.h"
#include "statement.h"

//-----------------------------------------------------------------------------
namespace DB {

	Line::Line( Manager &manager, const Endpoint &endpoint ) {
		m_connection = manager.Connect( endpoint );
	}
	
	std::unique_ptr<Statement> Line::CreateStatement() { 
		return std::unique_ptr<Statement>( new Statement(*this) );
	}
}
