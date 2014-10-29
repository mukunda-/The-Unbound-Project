//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

#pragma once

namespace Util {

	namespace {
		
		/// ------------------------------------------------------------------
		/// Convert degrees to radians.
		///
		template <typename T>
		inline T DegToRad( T degrees ) {
			return degrees * 0.01745329252f;
		}
		
		/// ------------------------------------------------------------------
		/// Convert radians to degrees.
		///
		template <typename T>
		inline T RadToDeg( T radians ) {
			return radians * 57.2957795130f;
		}
 
		const float PI = 3.14159265f;
	}

}

