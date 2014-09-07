//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//

#if 0 // replaced!

#ifndef PLANET_H
#define PLANET_H

#include "stdafx.h"

#define cplanet planet::world
/*
#define CHUNK_UNKNOWN ((planet::chunk*)0)	// chunk needs to be generated
#define CHUNK_EMPTY ((planet::chunk*)1)		// chunk is empty
#define CHUNK_SERVER ((planet::chunk*)2)	// chunk must be retrieved from server
#define CHUNK_JOB ((planet::chunk*)3)		// chunk has job pending
*/

typedef volatile unsigned int CHUNKPTR;
// CHUNKPTR
// lower byte = state
// upper tbyte = index
// 0-15: special code (see CHUNK_xxx) enum

namespace planet {



enum {
	SEED_LENGTH = 0x4000,
	SEED_MASK = 0x3FFF,
//	PLANET_WIDTH = 16384, // DEBUG, SMALLMAP! must be power of 2
//	PLANET_HEIGHT = 1024, // must be power of 2
//	PLANET_MASK = PLANET_WIDTH-1,
//	PLANET_WIDTH16 = 1024,
//	PLANET_HEIGHT16 = 64,
//	PLANET_WIDTH_BITS = 10,
//	PLANET_HEIGHT_BASE = PLANET_HEIGHT/2,
	CHUNK_ADDRESS_RESERVE = 16,
	//NETWORK_REQUEST_NEEDED = 1,
	//NETWORK_REQUEST_ISSUED = 2,

	//TEMPERATURE_MAP_WIDTH = 128,
	//PSCALE=2
};

enum {
	CHUNK_INVALID = 0,	// chunk must be requested from server (single player: treat as dormant)
	CHUNK_EMPTY = 1,	// chunk is known to be empty
	CHUNK_DORMANT = 2,	// chunk must be generated
	CHUNK_DISK = 3,		// chunk is stored on disk
	CHUNK_BUSY = 4,		// chunk state is busy and will be changed, check back later (address may not be valid)
	CHUNK_READY = 8,	// chunk is ready to be read
						// NOTE: cached blocks may be deleted with allocation calls
						// ensure data exists after create/delete routines
};

enum {
	CHUNK_SOLID_LEFT = 1,
	CHUNK_SOLID_RIGHT = 2,
	CHUNK_SOLID_FRONT = 4,
	CHUNK_SOLID_BACK = 8,
	CHUNK_SOLID_TOP = 16,
	CHUNK_SOLID_BOTTOM = 32,
	CHUNK_SOLID_ALL = 64-1
};

enum {
	CELL_RESERVED_00,	CELL_GRASS,			CELL_DIRT,			CELL_SAND,
	CELL_ROCK,			CELL_TREE,			CELL_FORESTGROUND,	CELL_BRICKS1,
	CELL_LIGHTBLOCK,		CELL_RESERVED_09,	CELL_GEMSTONE,		CELL_RESERVED_0B,
	CELL_AIR,			CELL_TALLGRASS,		CELL_RESERVED_0E,	CELL_LEAVES,

	CELL_MOUNTAIN1,		CELL_MOUNTAIN2,		CELL_MOUNTAIN3,		CELL_MOUNTAIN4,
	CELL_RENDERVOID,	CELL_RESERVED_15,	CELL_RESERVED_16,	CELL_RESERVED_17,
	CELL_RESERVED_18,	CELL_RESERVED_19,	CELL_RESERVED_1A,	CELL_RESERVED_1B,
	ffffff,				CELL_WATER,			CELL_RESERVED_1E,	CELL_RESERVED_1F,

};

enum { // BITGRID Y Z X
	bitgrid_bottom_back_left, bitgrid_bottom_back_center, bitgrid_bottom_back_right,
	bitgrid_bottom_center_left, bitgrid_bottom_center_center, bitgrid_bottom_center_right,
	bitgrid_bottom_front_left, bitgrid_bottom_front_center, bitgrid_bottom_front_right,
	
	bitgrid_center_back_left, bitgrid_center_back_center, bitgrid_center_back_right,
	bitgrid_center_center_left, bitgrid_center_center_center, bitgrid_center_center_right,
	bitgrid_center_front_left, bitgrid_center_front_center, bitgrid_center_front_right,

	bitgrid_top_back_left, bitgrid_top_back_center, bitgrid_top_back_right,
	bitgrid_top_center_left, bitgrid_top_center_center, bitgrid_top_center_right,
	bitgrid_top_front_left, bitgrid_top_front_center, bitgrid_top_front_right
};

enum {
	COLL_MISS, COLL_HIT, COLL_UNKNOWN
};

typedef struct t_dataentry_header {
	struct t_dataentry_header *next;
	u16 index; // index of block (yyyyzzzzxxxx)
	u16 size;
	u16 type;

	// cast for more data

} dataentry_header;

class chunktable {
	
public:
	CHUNKPTR *table;

	int width, height, length;
	int width16, height16, length16;
	bool valid;

	chunktable();
	~chunktable();
	void clean();
	void create( int width, int height, int length );
	void initialize_from_disk( const char *path );
	void reset();
	int  index_from_coords( int x, int y, int z );
	void coords_from_index( int index, int &x, int &y, int &z );
	CHUNKPTR set( int x, int y, int z, CHUNKPTR state );
	CHUNKPTR set( int index, CHUNKPTR state );
	CHUNKPTR invalidate( int x, int y, int z );
	CHUNKPTR invalidate( int index );
	CHUNKPTR get( int x, int y, int z );
	CHUNKPTR get( int index ); 
	CHUNKPTR * get_pointer( int x, int y, int z );

};
/*
typedef struct t_sample2d {
	u8 a; // a | b
	u8 b; // --+--
	u8 c; // c | d
	u8 d;
} sample2d;
*/

typedef struct t_sample3d {
	s8 data[4]; // x*4 (uses next sample) + y*2 + z
} sample3d;

//typedef struct t_map_param {
//	u8 params[8];
	/*
	u8 elevation;
	u8 turba;
	u8 turbf;
	u8 density;
	u8 mountains;
	u8 trees;
	u8 temperature;
	u8 rainfall;
	u8 saltwater;
	u8 fog;
	u8 cave1,cave2,cave3,cave4;
	u8 interpolation;
	u8 filler1;*/
//} map_param;

typedef struct t_map_param {
	u8 params[4];
} map_param;

typedef struct t_map_param_h {
	u8 params[4];
} map_param_h;

enum {
	MPH_HEIGHT, MPH_OVERLAY, MPH_SEALEVEL, MPH_ZONES
};

enum {
	MP_ZONE, MP_TEMPERATURE, MP_CAVES, MP_RESERVED
};

enum {
	C1_ZONE_NULL, C1_ZONE_PLAINS, C1_ZONE_FOREST, C1_ZONE_MOUNTAINS,
	C1_ZONE_BEACH, C1_ZONE_SEA
};

//	MP_ELEVATION,	MP_ZONE,	MP_TEMPERATURE,	MP_CAVES,	MP_SEADISTANCE,	MP_RESERVED1,	MP_RESERVED2,	MP_RESERVED3
	/*
	MP_ELEVATION,	MP_TURBA,	MP_TURBF,		MP_DENSITY, 
	MP_MOUNTAINS,	MP_TREES,	MP_TEMPERATURE,	MP_RAINFALL,
	MP_SALTWATER,	MP_FOG,		MP_CAVE1,		MP_CAVE2,
	MP_CAVE3,		MP_CAVE4,	MP_INTERP,		MP_RESV1*/
//};

enum {
	INTERPOLATION_NONE,
	INTERPOLATION_LINEAR,
	INTERPOLATION_COSINE
};

//-----------------------------------------------------------------------------
class world_parameters {
//-----------------------------------------------------------------------------
	 
	void clean();
	void compute_sealevel();
	void upscale_zone_forms();
	void paint_stamp( int x, int z, u8 *stamp, u8 *temp );
	bool load_zoneforms_cache();
	void save_zoneforms_cache();
	bool load_sealevel_cache();
	void save_sealevel_cache();
	std::string zoneform_cache_path();
	std::string sealevel_cache_path();
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

	world_parameters();
	~world_parameters();

	void load( const char *file );
	map_param *get_data();
	map_param_h *get_data_h();
	map_param *read_data(int x, int y);
	map_param_h * read_data_h( int xi, int zi, int x, int z );
	
};

//-----------------------------------------------------------------------------
class context {
//-----------------------------------------------------------------------------

public:
	chunktable chunks;
	world_parameters map;
	const char *datapath;

	int width,depth,length;
	int width16,depth16,length16;
	//int depth_base;
	int sea_level;
	 
	CRITICAL_SECTION cs_regs;

	context();
	~context();
	void initialize( const char *mapfile, const char *regionfiles );
	 
};

__declspec(align(16)) struct chunk {
	u8		data[16*16*16];
	//u8		sunlight[16*16*16];		//  0 - 12
	
	context *ct;
	int		table_index;
	u16		memory_index;
//	u16		instance;				// clientside only
	u8		busy;					// chunk is being modified (in another thread)
//	u8		solid;					// 
	u8		touched;				// needs saving
//	u8		dirty;					// regenerate geometry
//	u8		sunlight_changed;		// update sunlight cache MOVE TO RENDERING INSTANCE
//	u8		dimlight_changed;		// update dimlight cache MOVE TO RENDERING INSTANCE
	u8		modified;				// (recheck solid bitmap [?])
//	u8		empty;
//	u8		sun_init;				// sunlight wasnt initialized (do not use as sun stamp)
//	u8		dirty_sides;			// new chunk, adjacent chunks are dirty.
//	u8		all_water;				// every cube is water
	
	dataentry_header *data_list;	// used serverside only
									// client uses function

	struct chunk *prev;
	struct chunk *next;
};

typedef struct t_rawchunk {
	u8		data[16*16*16];
	dataentry_header *data_list;
} rawchunk;

typedef struct t_compressedchunk {
	u32		datasize;
	u8		data[16*16*16*2]; // maximum possible size
	dataentry_header *data_list;

} compressedchunk;

//void use_mapfile( const char *mapfile );
//void create_network( int seed );

void set_context( context * );

CHUNKPTR *get_chunk_slot( int x, int y, int z );
CHUNKPTR get_chunk_pointer( int x, int y, int z );
//CHUNKPTR get_chunk_pointer_wrapped( int x, int y, int z );
CHUNKPTR get_chunk_pointer( int world_index );
chunk *get_chunk( CHUNKPTR );

float get_sea_level();

void refresh_chunk( CHUNKPTR );

int get_width(); // x
int get_depth(); // y
int get_length(); // z


int get_cube( int x, int y, int z );

bool request( int x, int y, int z );
bool request( int world_index );

bool collision( int x, int y, int z );
bool collision3f( cml::vector3f pos );
bool collision3d( cml::vector3d pos );
 
//void recalc_sunlight( int x, int z );
//bool is_sunlight_dirty( int x, int z );

void destroy_radius( int x, int y, int z, int radius );
void process_mods();

//-------------------------------------------------------------------------------------------------
static inline bool real_chunk( CHUNKPTR ch ) {
//-------------------------------------------------------------------------------------------------
	return !!((ch&0xFF) == CHUNK_READY);
}

//-------------------------------------------------------------------------------------------------
//static inline int getWorldIndex( int x, int y, int z ) {
//-------------------------------------------------------------------------------------------------
//	return x + (y*(PLANET_WIDTH16)*(PLANET_WIDTH16)) + (z*PLANET_WIDTH16);
//}

//-------------------------------------------------------------------------------------------------
static inline int read_state( CHUNKPTR cs ) {
//-------------------------------------------------------------------------------------------------
	return cs & 0xFF;
}

//void destroy_radius( int x, int y, int z, int radius );
//void set_cell( cml::vector3i pos, int cell );

void test_solid_faces( chunk *ch );
int get_opacity( chunk *ch, int cx, int cy, int cz );

void do_render_jobs();

bool pick( const cml::vector3f &start, const cml::vector3f &end, cml::vector3f *intersection, cml::vector3i *collided, cml::vector3i *adjacent, cml::vector3f *normal );
 

u8 random_number();

typedef struct t_job {
	int type;
	chunk *ch;
	context *ct;
	int index;
	int data1;
	struct t_job * next;
} job;


//void queue_generation( int x, int y, int z );
//void queue_lighting( int x1, int y1, int z1, int x2, int y2, int z2 );
//void do_job( job &a );
//void do_jobs();


void initialize();
void unload();

};

#endif

#endif
