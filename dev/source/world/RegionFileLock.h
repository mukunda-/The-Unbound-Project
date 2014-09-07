//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//

#pragma once

#include "world/Context.h"

namespace World {

class RegionFileLock {
	Context *context;
	int region_index;
	
public:
	RegionFileLock( Context *ct, int region_index );
	~RegionFileLock();
};

}
