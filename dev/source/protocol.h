//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright � 2014, Mukunda Johnson, All rights reserved. =========//

// Network Protocol Identifiers

#pragma once

namespace Net { namespace Proto { 
	
	namespace ID {

		namespace Groups {
			namespace {
				const int AUTH = 0x3000;
			}
		}
			
		namespace Auth {
			const int LOGIN = Groups::AUTH | 0x01;
		}
	}
	 
}}