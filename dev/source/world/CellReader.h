//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2016, Mukunda Johnson, All rights reserved. ==========//

// this is a class that eases the process of reading data from a cell or cells
// it automatically locks and unlocks the cells it is accessing
//
// please note that locks should be released as soon as possible / instances
// of this class should not be kept idle
//
#pragma once

#include <boost/cstdint.hpp>

namespace World {

class CellReader {

	bool lock_zones;

	bool locked;
	boost::uint64_t lock_shuffled;
	boost::uint64_t lock_address;

	Cell *current_cell;
	Context *context;

public:

	// construct cell reader
	// if zone_based is set, this reader will lock entire zones when accessing data
	// (to increase efficiency when doing operations on large areas)
	//
	// NOTE: zone_based is not implemented yet
	//
	CellReader( Context &ct, bool zone_based = false );
	~CellReader();

	// read a cell from coordinates or a world index
	// coordinates are in cell units
	Cell *GetCell( int x, int y, int z );
	Cell *GetCell( boost::uint64_t world_index );

	// read a cube from coordinates or a world index + offset
	// coordinates are in cube units
	boost::uint16_t GetCube( int x, int y, int z );
	boost::uint16_t GetCube( boost::uint64_t world_index, int local_index );
};

}
