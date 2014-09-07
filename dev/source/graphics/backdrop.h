//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//
#pragma once

#ifndef BACKDROP_H
#define BACKDROP_H

namespace backdrop {

void init( );
void render();
void render_sun(bool moon);
void render_sunflare();
void set_time( float tod );
}

#endif
