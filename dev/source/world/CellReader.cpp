//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//

//-------------------------------------------------------------------------------------------------
#include <stdafx.h>

//-------------------------------------------------------------------------------------------------
namespace World {


//-------------------------------------------------------------------------------------------------
CellReader::CellReader( Context &ct, bool zone_based ) {
	lock_zones = zone_based;
	locked = false;
	lock_shuffled = -1;
	lock_address = -1;
	current_cell = 0;
	context = &ct;
}

//-------------------------------------------------------------------------------------------------
CellReader::~CellReader() {
	if( current_cell ) {
		context->GetCells()->Release(lock_address);
	}
}

//-------------------------------------------------------------------------------------------------
Cell *CellReader::GetCell( int x, int y, int z ) {
	
	return GetCell( PackCoords(x,y,z) );
}

//-------------------------------------------------------------------------------------------------
Cell *CellReader::GetCell( boost::uint64_t world_index ) {
	// if address matches, return current cell
	if( world_index == lock_address ) {
		return current_cell;
	}

	// unlock current cell if exists
	if( current_cell ) {
		// unlock
		context->GetCells()->ReleaseS( lock_shuffled );
	}

	// copy address
	lock_address = world_index;
	lock_shuffled = context->GetCells()->ShuffleCoords(world_index);
	
	// acquire lock on new cell and request it if it isn't ready
	int state;
	context->GetCells()->AcquireS( lock_shuffled, &current_cell, &state );
	if( !CellStateReady(state) ) {
		context->RequestWait( lock_address );
	}

	return current_cell;
}

//-------------------------------------------------------------------------------------------------
boost::uint16_t CellReader::GetCube( int x, int y, int z ) {
	return GetCell( x>>4, y>>4, z>>4 )->cubes[ (x&15) + (y&15)*256 + (z&15)*16 ];
}

//-------------------------------------------------------------------------------------------------
boost::uint16_t CellReader::GetCube( boost::uint64_t world_index, int local_index ) {
	return GetCell( world_index )->cubes[local_index];
}

}

