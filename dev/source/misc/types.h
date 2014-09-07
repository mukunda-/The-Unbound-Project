//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//

#pragma once

#ifndef ABCPAIOHEGAIOAWEGTYPES_H
#define ABCPAIOHEGAIOAWEGTYPES_H

#include <boost/cstdint.hpp>

typedef boost::uint8_t u8;
typedef boost::uint16_t u16;
typedef boost::uint32_t u32;


typedef boost::int8_t s8;
typedef boost::int16_t s16;
typedef boost::int32_t s32;

typedef struct t_vectorInt {
	int x, y, z;
} vectorInt;

//typedef struct t_vector64 {
//	double x, y, z;
//} vector64;


typedef struct t_matrix {
	float xx, xy, xz;
	float yx, yy, yz;
	float zx, zy, zz;
} matrix;
/*
static inline void set_vector64( vector64 *dest, double x, double y, double z ) {
	dest->x = x;
	dest->y = y;
	dest->z = z;
}*/

static inline float random( float low, float high ) {
	float r = (float)rand() / RAND_MAX;
	return (high-low)*r+low;
}

static inline int randint( int low, int high ) {
	float r = (float)rand() / RAND_MAX;
	return (int)floor((float)(high-low)*r + 0.5)+low;
}

#endif
