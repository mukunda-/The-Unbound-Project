//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//

//-------------------------------------------------------------------------------------------------
#pragma once

//-------------------------------------------------------------------------------------------------
#ifndef GRAPHICS_WORLD_RENDERING_INSTANCE_H
#define GRAPHICS_WORLD_RENDERING_INSTANCE_H

#include "misc/types.h"

//-------------------------------------------------------------------------------------------------
namespace Graphics {

class WorldRenderingList;

//-------------------------------------------------------------------------------------------------
class WorldRenderingInstance {
	
	WorldRenderingList *parent;

public:
	// currently registered with a rendering list
	bool registered;

	// position in instance units (chunks/2)
	int x, y, z;

	// index into rendering instance map
	int map_index;

	// size of vertex data
	int vbosize;

	// size of water/liquid (translucent) vertex data
	int vbosize_water;

	// ???
	int index;
	
	// delay before updates occur
	// to avoid nearby data to form before generating vertex data
	int gdelay;

	// opacity for fading effects
	float opacity;

	// cube data changed
	bool dirty;

	// light changed flags
	// for updating the light cache
	bool skylight_changed;
	bool dimlight_changed;

	// the light cache
	// 4 bytes in r,g,b,s order
	// r,g,b = dimlight components
	// s = skylight intensity
	u32 light_cache[40*34*34];

	// list that contains this instance
	// 0 if unlinked

	WorldRenderingInstance *next;
	WorldRenderingInstance *prev;

	WorldRenderingInstance();
	~WorldRenderingInstance();
	
	void SetParent( WorldRenderingList *p_parent );
	WorldRenderingList *GetParent();
};

//-------------------------------------------------------------------------------------------------
} //namespace

//-------------------------------------------------------------------------------------------------
#endif
