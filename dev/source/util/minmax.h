//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//

#pragma once

namespace Util {

template<class T> inline const T& Max(const T& a, const T& b) {

    return b < a ? a : b;
}

template<class T> inline const T& Min(const T& a, const T& b) {

    return b > a ? a : b;
}

template<class T> inline const T& Clamp(const T& a, const T& min, const T& max ) {
	if( a < min ) return min;
	if( a > max ) return max;
	return a;
}

}