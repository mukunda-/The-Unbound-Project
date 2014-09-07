//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//

#pragma once

#ifndef RANDOM_H
#define RANDOM_H

namespace rnd {


// start PRNG generator
void reset( int seed );

// get next random number
int next();

// get upper limit of the output
int getmax();

// get normalized random number (float/double format)
float next_float();
double next_double();

void setup_table();
float get_static( float index );

}

#endif
