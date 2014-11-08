//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//

#pragma once

#include "system/system.h"

namespace Net {

	System::Service &DefaultService(); 
	void Run( int threads );

	struct Instance {
	 
		Instance( int threads );
		~Instance();

		System::Service &GetService();

	private:
	
		System::Service m_service;
	};

}
