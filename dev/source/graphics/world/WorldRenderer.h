//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//

#pragma once

//-------------------------------------------------------------------------------------------------
#ifndef GRAPHICS_WORLDRENDERER_H
#define GRAPHICS_WORLDRENDERER_H

//-------------------------------------------------------------------------------------------------
#include "util/linkedlist.h"
#include "misc/types.h"
#include "graphics/world/SkylightManager.h"
#include "graphics/world/WorldRenderingList.h"

//-------------------------------------------------------------------------------------------------
namespace Graphics {

//-------------------------------------------------------------------------------------------------
class WorldRenderer {

	SkylightManager *skylight_manager;
	WorldRenderingList *rendering_list;

	World::Context *context;

	void RunFrontScanner();
	void RunBackScanner();
public:
	WorldRenderer();
	~WorldRenderer();

	// update function, runs scanning, prepares geometry, etc!
	// all with multithreading and flying colors!
	void Update();

	// rendering function, simply amazing!
	void Render();
};

//-------------------------------------------------------------------------------------------------
}

//-------------------------------------------------------------------------------------------------
#endif