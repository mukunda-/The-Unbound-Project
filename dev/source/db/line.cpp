//===========================  The Unbound Project  =========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

#include "stdafx.h"
#include "core.h"
#include "line.h"

//-----------------------------------------------------------------------------
namespace DB {

	Line::Line( Manager &manager, const Endpoint &endpoint ) {
		m_connection = manager.Connect( endpoint );
	}
}
