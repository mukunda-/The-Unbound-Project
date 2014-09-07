//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//

// random number generator

#include "stdafx.h"

namespace rnd {

float seeded[4097];

void reset( int seed ) {
	srand(seed);
}

int next() {
	return rand();
}

int get_max() {
	return RAND_MAX;
}

float next_float() {
	return (float)rand() / (float)RAND_MAX;
}

double next_double() {
	return (double)rand() / (double)RAND_MAX;
}

void setup_table() { 
	for( int i = 0; i < 4096; i++ ) {
		seeded[i] = next_float();
	}
	seeded[4096] = seeded[0];
}

float get_static( float index ) {
	int a = (int)floor(index);
	a = a & 4095;
	
	float mu = index - (float)a;
	mu = (1-cos(mu*3.14159f))/2;
	return seeded[a] + (seeded[a+1]-seeded[a]) * mu;
}

}
