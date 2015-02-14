//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

#include <stdafx.h>

#include "core.h"
#include "work.h"

namespace Net {

extern Instance *g_instance;

Work::Work() {
	g_instance->AddWork();
}

Work::~Work() {
	
	g_instance->RemoveWork();
}

}

