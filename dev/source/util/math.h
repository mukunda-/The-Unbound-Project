//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

#pragma once

namespace Util {

	namespace {

		inline float DegToRad( float degrees ) {
			return degrees / 180.0f * 3.14159265f;
		}
		
		inline float RadToDeg( float radians ) {
			return radians * 180.0f / 3.14159265f;
		}
 
		const float PI = 3.14159265f;
	}

}

