//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//

#pragma once

#ifndef CLOUDS_H
#define CLOUDS_H

namespace clouds {

void init();
void update();
void render();

void set_density( float d );
void set_color( float r, float g, float b );
void set_center( float x, float z );
void set_light( float light );

}

#endif
