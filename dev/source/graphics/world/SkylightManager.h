//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//

//-------------------------------------------------------------------------------------------------
#pragma once

//-------------------------------------------------------------------------------------------------
#ifndef GRAPHICS_WORLD_SKYLIGHTMANAGER_H
#define GRAPHICS_WORLD_SKYLIGHTMANAGER_H

//-------------------------------------------------------------------------------------------------
#include "misc/types.h"
#include "util/LinkedList.h"
#include "world/Context.h"

//-------------------------------------------------------------------------------------------------
namespace Graphics {

//-------------------------------------------------------------------------------------------------
typedef struct t_SkylightData {
	int references;			// number of instances using this data
	int index;				// not used
	s16 dirty[4];			// -1 = not dirty, 0x7FFF = FULL DIRTY! MEASURED IN CHUNKS

	u16 depth[16*16*4];		// depth map for 4 world slices
	u16 lowest_depth[4];	// lowest point for each world slice

	struct t_SkylightData *next;	// linked list data
	struct t_SkylightData *prev;	//
} SkylightData;

//-------------------------------------------------------------------------------------------------
class SkylightManager {

	LinkedList<SkylightData> instances;
	
	//---------------------------------------------------------------------------------------------
	// map for associating locations with instances
	//
	SkylightData *data_map;
	int data_map_width;
	int data_map_length;
	
	//---------------------------------------------------------------------------------------------
	// world context being used
	//
	World::Context *context;

	//---------------------------------------------------------------------------------------------
	// add a new instance to the instance list
	//
	SkylightData *AddNewInstance();

	//---------------------------------------------------------------------------------------------
	// delete an instance from the list
	//
	void DeleteInstance( SkylightData *instance );
	
	//---------------------------------------------------------------------------------------------
	// get an instance located at the coordinates
	// returns 0 if none registered
	//
	SkylightData *GetInstance( int x, int z );
	
	//---------------------------------------------------------------------------------------------
	// this variant checks if the x and z coordinates are within the data map boundaries
	// the other does not
	SkylightData *GetInstanceVerify( int x, int z );

	//---------------------------------------------------------------------------------------------
	// update the depth data for a single world slice
	//
	// ix,iz = offset within instance (0 or 1)
	// offset = ???
	//
	void UpdateDepthSingle( SkylightData *instance, int ix, int iz, int offset );

	//---------------------------------------------------------------------------------------------
public:
	SkylightManager();
	~SkylightManager();

	//---------------------------------------------------------------------------------------------
	// setup the skylight manager
	// allocates the data map with dimensions specified
	//
	void Setup( int width, int length );

	//---------------------------------------------------------------------------------------------
	// erases all data
	//
	void Reset();

	//---------------------------------------------------------------------------------------------
	// for when the client enters a new world
	//
	void ChangeContext( World::Context *p_context );
		
	//---------------------------------------------------------------------------------------------
	// register a location that is going to be used (by a rendering instance)
	//
	// the internal reference counter is increased for each instance registering
	// the same location
	//
	// returns the created or existing skylight data associated with the location
	//
	// parameters are measured in instance units, not chunk units
	// (ie chunks/2)
	//
	SkylightData *Register( int x, int z );

	//---------------------------------------------------------------------------------------------
	// unregister a location, the data for the location is deleted if the
	// registered reference count reaches 0
	//
	// parameters are in instance units
	//
	void Unregister( int x, int z );

	//---------------------------------------------------------------------------------------------
	// set the dirty flag for the engine to refresh an area
	// should be set after anything affecting the chunk geometry changes
	//
	// parameters are in chunk units
	//
	void SetDirty( int x, int y, int z );	

	//---------------------------------------------------------------------------------------------
	// update skylight data for an instance slice
	// 
	// call for each active rendering instance
	//
	// paramters are in instance units
	//
	void Update( int x, int z );

	//---------------------------------------------------------------------------------------------
	// checks if a world slice is flagged dirty
	//
	// x,z are in chunk units
	//
	bool IsDirty( int x, int z );

	//---------------------------------------------------------------------------------------------
	// get skylight depth data
	//
	// this is for the rendering engine
	//
	// returns a 16x16 map of skylight depth (one per cube)
	//
	u16 *GetData( int x, int z );
};

}

#endif
