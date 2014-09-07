//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//
#pragma once

#ifndef PARTICLES_H
#define PARTICLES_H

namespace particles {

//particle kernel listing, generated 12:00:00 AM
enum {
    //PK_RAINDROP,
    PK_SMOKEY,
    PK_BIGSMOKEY,
	PK_FIRE,
	PK_GRASS_PARTICLES,
	PK_DIRT_PARTICLES,
	PK_FLARES1,
	PK_BIGBLACKSMOKEY
};

void add( u8 kernel, cml::vector3f pos, cml::vector3f velocity );
void add( u8 kernel, cml::vector3f pos );
void update();
void draw();
void init();

}

#endif
