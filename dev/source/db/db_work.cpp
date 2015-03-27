//===========================  The Unbound Project  =========================//
//                                                                           //
//========= Copyright © 2015, Mukunda Johnson, All rights reserved. =========//

#include <stdafx.h>
#include "work.h"
#include "core.h"

namespace DB {
	
	extern Manager *g_instance;

	//-------------------------------------------------------------------------
	Work::Work() {
		g_instance->WorkAdded();
	}

	//-------------------------------------------------------------------------
	Work::~Work() {
		g_instance->WorkCompleted();
	}
}