//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//
#pragma once

#include <boost/cstdint.hpp>

namespace World {

//-------------------------------------------------------------------------------------------------
// return world index
//
static inline boost::uint64_t PackCoords( int x, int y, int z ) {
	return x + (z<<16) + ((boost::uint64_t)y<<32);
}

//-------------------------------------------------------------------------------------------------
// return world coordinates
//
static inline void UnpackCoords( boost::uint64_t packed, int &x, int &y, int &z ) {
	x = packed & 0xFFFF;
	z = (packed >> 16) & 0xFFFF;
	y = (packed >> 32) & 0xFFFF;
}


}
