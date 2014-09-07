//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//

#pragma once

#include <boost/cstdint.hpp>
#include "world/Generation.h"

namespace World {

//------------------------------------------------------------------------------------------------------------------------
// entry for low-resolution maps (1 per chunk)
typedef struct t_map_param {
	boost::uint8_t params[4];
} map_param;

//------------------------------------------------------------------------------------------------------------------------
// entry for high-resolution maps (16x16 per chunk, 1 per square unit)
typedef struct t_map_param_h {
	boost::uint8_t params[4];
} map_param_h;

//------------------------------------------------------------------------------------------------------------------------
// definitions for the low resolution map entries
enum {
	MP_ZONE, MP_TEMPERATURE, MP_CAVES, MP_RESERVED
};

//------------------------------------------------------------------------------------------------------------------------
// definitions for the high resolution map entries
enum {
	MPH_HEIGHT, MPH_OVERLAY, MPH_SEALEVEL, MPH_ZONES
};

//------------------------------------------------------------------------------------------------------------------------
// WorldParameters
// manages map data used in terrain generation and weather control
//
class WorldParameters {
	 
	// delete and invalidate data
	void Clean();
	
	// stamp painting function used during computations
	void PaintStamp( int x, int z, boost::uint8_t *stamp, boost::uint8_t *temp );

	// scan over terrain data finding sea level data (slow! use cache functions!)
	void ComputeSeaLevel();

	// scan over terrain data creating higher resolution forms (slow! use cache functions!)
	void ComputeZoneForms();

	// skip zone form computation by loading previous data from disk
	// returns false if cache doesn't exist
	bool LoadZoneFormsCache();

	// save zone forms to disk
	void SaveZoneFormsCache();

	// skip sea level computation by loading previous data from disk
	// returns false if cache doesn't exist
	bool LoadSeaLevelCache();

	// save sea level data to disk
	void SaveSeaLevelCache();
	
	// get path to cache files
	std::string ZoneFormsCachePath();
	std::string SeaLevelCachePath();
public:

	std::string worldname;

	map_param *data;
	map_param_h *data_h; // hi-res !!!
	short width;	// x
	short length;	// z
	short depth;	// y
	short kernel;
	short sea_level;
	bool loaded;
	std::string filepath;

	WorldParameters();
	~WorldParameters();

	// load map file
	void Load( const char *file );

	// low-resolution data access
	const map_param *GetData() const;

	// high-resolution data access
	const map_param_h *GetDataH() const;

	// read single cell with boundary protection (returns 0 if out of bounds)
	const map_param   *ReadData(int x, int y) const;
	const map_param_h *ReadDataH( int xi, int zi, int x, int z ) const;
	
};
}
