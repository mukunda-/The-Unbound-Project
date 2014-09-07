//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//

#pragma once

#include "system/system.h"

namespace Network {


System::Service &DefaultService();

void Run( int threads );

struct Init {
	 
	Init( int threads );
	~Init();
};

//extern Service DEFAULT_SERVICE;

}
