//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//

#pragma once

#include <boost/cstdint.hpp>
#include "util/stringtable.h"

namespace World {

	//-----------------------------------------------------------------------------------------------------------------
	typedef struct t_CropData {
		boost::uint16_t conditions;	// index into conditions table
		boost::uint16_t yield;
		boost::uint16_t resist_pests;
		boost::uint16_t resist_disease;
		boost::uint16_t water_saturation_comfort;
		boost::uint16_t hunger; // need nice word for how much of the land's fertility it uses up
	} CropData;

	//-----------------------------------------------------------------------------------------------------------------
	typedef struct t_CubeSideProperties {
		boost::uint16_t spring;
		boost::uint16_t friction;
		boost::uint16_t surface_type;
		boost::uint16_t damage;    // damage/trap/impale
		boost::uint16_t holy;     // consecrated ground
		boost::uint16_t texture; //
		boost::uint16_t shielded;           //todo: shield types
		boost::uint16_t move_bonus;        //movement speed bonus while standing on it
		boost::uint16_t light_absorption; // for colored glass
		bool paintable;
	} CubeSideProperties;

	//-----------------------------------------------------------------------------------------------------------------
	typedef struct t_CubeProperties {
		char name[32];
		int namehash;
		char description[64];
		boost::int32_t cost;
		bool clutter;			// "cube or clutter"
		// todo: geometry data
		boost::uint8_t light[4]; // light emittance
		boost::uint16_t blast_resistance; // 0-256
		boost::uint16_t blast_resistance_r;
		boost::int32_t flammability;
		boost::int16_t material; // shatter material
		boost::int16_t physics;
		boost::int16_t liquid;
		boost::int16_t emitter; // index of particle emitter
		bool passable;
		boost::uint16_t collision_map[4]; // collision bitmap (64 bits)
		bool item;
		boost::int16_t explosive;
		CropData *crop_data;

		CubeSideProperties sides[6];
	} CubeProperties;

	class CubeKernel;

	//-----------------------------------------------------------------------------------------------------------------
	typedef struct t_CubeKernelParserData {
		CubeKernel *parent;
		std::vector<CubeProperties> classes;
		std::vector<CubeProperties> instances;

		CubeProperties *current;
	} CubeKernelParserData;

	//-----------------------------------------------------------------------------------------------------------------
	class CubeKernel {
		
		int num_types;
		CubeProperties *props;
		char name[64];

		Util::StringTable st_props;

	public:
		CubeKernel( const char *p_file );
		~CubeKernel();

		const CubeProperties *GetData(int index) const;
		int FindDataIndex( const char *name ) const;
	};

}
