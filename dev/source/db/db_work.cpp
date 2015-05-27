//===========================  The Unbound Project  =========================//
//                                                                           //
//========= Copyright © 2015, Mukunda Johnson, All rights reserved. =========//

#include <stdafx.h>
#include "work.h"
#include "core.h"

namespace DB {
	
	extern Manager *g_manager;

	//-------------------------------------------------------------------------
	Work::Work() {
		g_manager->AddWork();
	}

	//-------------------------------------------------------------------------
	Work::~Work() {
		g_manager->RemoveWork();
	}
}
