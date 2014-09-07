//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//
// simple class for containing rendering instances
// for managing and sorting them

//-------------------------------------------------------------------------------------------------
#pragma once

//-------------------------------------------------------------------------------------------------
#ifndef GRAPHICS_WORLD_RENDERING_LIST_H
#define GRAPHICS_WORLD_RENDERING_LIST_H

//-------------------------------------------------------------------------------------------------
#include "util/LinkedList.h"
#include "graphics/world/WorldRenderingInstance.h"

//-------------------------------------------------------------------------------------------------
namespace Graphics {

//-------------------------------------------------------------------------------------------------
class WorldRenderingList {
	LinkedList<WorldRenderingInstance> list;

public:
	void Erase();
	void Add( WorldRenderingInstance *i );
	WorldRenderingInstance *Remove( WorldRenderingInstance *i );
	WorldRenderingInstance *GetFirst();
	WorldRenderingInstance *GetLast();
	WorldRenderingInstance *PopFirst();
	bool IsEmpty() const;
	
};

//-------------------------------------------------------------------------------------------------
} //namespace

//-------------------------------------------------------------------------------------------------
#endif
