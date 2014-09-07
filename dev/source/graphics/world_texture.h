//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//

#pragma once

#ifndef WORLDTEXTURE_H
#define WORLDTEXTURE_H

namespace world_texture {

enum {
	HD_GRASS = 256,
	HD_DIRT = HD_GRASS+16,
	HD_ROCK = HD_DIRT+16
};

void create();

}

#endif

