//===========================  The Unbound Project  =========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

#include "stdafx.h"
#include "core.h"
#include "line.h"

//-----------------------------------------------------------------------------
namespace DB {
	extern Instance *g_instance;

	Line::Line( const Endpoint &endpoint ) {
		m_connection = g_instance->Connect( endpoint );
	}
}
