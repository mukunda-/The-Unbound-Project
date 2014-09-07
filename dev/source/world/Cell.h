//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//

#pragma once

#include <boost/thread.hpp>
namespace World {
	
//------------------------------------------------------------------------------------------------------------------------
enum {
	// CELLSTATE BIT FORMATTING:
	//  r-mlecss
	//    s:inactive state setting
	//    c:creation flag
	//    e:empty flag
	//    l:exclusive reserve flag
	//    m:exclusive lock flag
	//    -:unused
	//    r:ready flag

	// UNKNOWN (inactive state)
	//   client: cell data must be requested from server
	//   server: cell state is either DORMANT or DISK
	CELLSTATE_UNKNOWN = 0,
	
	// DORMANT (inactive state): cell needs to be generated and has no modifications
	CELLSTATE_DORMANT,
	
	// DISK (inactive state): cell modifications are saved to disk (server only)
	CELLSTATE_DISK,
	
	// flag for managing the creation procedure
	// signals system that the cell has been acquired for creation and no other threads may
	// process the chunk at this time
	CELLSTATE_CREATION = 4,	

	// EMPTY: the cell is known to contain all ZERO cubes and does not have an actual Chunk allocated for it
	CELLSTATE_EMPTY = 8,

	// EXCLUSIVE RESERVE:
	CELLSTATE_RESERVED = 16,

	// EXCLUSIVE LOCK:
	CELLSTATE_LOCKED = 32,
	
	// READY: cell data is known
	CELLSTATE_READY = 128,

	// NOTES:
	// INACTIVE states may revert to "UNKNOWN" if the cell tree DROPS the memory for it
	// at any time, a CREATING cell may become READY
	// slots holding unlocked READY cellS may revert to an inactive state at any time the chunk manager
	// decides to free the memory for it

};

//------------------------------------------------------------------------------------------------------------------------
static inline int CellStateEmpty( int state ) {
	return state & CELLSTATE_EMPTY;
}

//------------------------------------------------------------------------------------------------------------------------
static inline int CellStateCreation( int state ) {
	return state & CELLSTATE_CREATION;
}

//------------------------------------------------------------------------------------------------------------------------
static inline int CellStateReady( int state ) {
	return state & CELLSTATE_READY;
}

//------------------------------------------------------------------------------------------------------------------------
static inline int ReadInactiveState( int state ) {

	return state & 3;
}

//------------------------------------------------------------------------------------------------------------------------
enum { // BITGRID Y Z X
	BITGRID_BOTTOM_BACK_LEFT, BITGRID_BOTTOM_BACK_CENTER, BITGRID_BOTTOM_BACK_RIGHT,
	BITGRID_BOTTOM_CENTER_LEFT, BITGRID_BOTTOM_CENTER_CENTER, BITGRID_BOTTOM_CENTER_RIGHT,
	BITGRID_BOTTOM_FRONT_LEFT, BITGRID_BOTTOM_FRONT_CENTER, BITGRID_BOTTOM_FRONT_RIGHT,
	
	BITGRID_CENTER_BACK_LEFT, BITGRID_CENTER_BACK_CENTER, BITGRID_CENTER_BACK_RIGHT,
	BITGRID_CENTER_CENTER_LEFT, BITGRID_CENTER_CENTER_CENTER, BITGRID_CENTER_CENTER_RIGHT,
	BITGRID_CENTER_FRONT_LEFT, BITGRID_CENTER_FRONT_CENTER, BITGRID_CENTER_FRONT_RIGHT,

	BITGRID_TOP_BACK_LEFT, BITGRID_TOP_BACK_CENTER, BITGRID_TOP_BACK_RIGHT,
	BITGRID_TOP_CENTER_LEFT, BITGRID_TOP_CENTER_CENTER, BITGRID_TOP_CENTER_RIGHT,
	BITGRID_TOP_FRONT_LEFT, BITGRID_TOP_FRONT_CENTER, BITGRID_TOP_FRONT_RIGHT
};

class Context;

enum {
	COMP_DAMAGE,
	COMP_PAINT,
//	COMP_PAINT2,
//	COMP_PAINT3,
//	COMP_PAINT4,
//	COMP_PAINT5,
//	COMP_PAINT6,
	COMP_DECAL,
//	COMP_DECAL2,
//	COMP_DECAL3,
//	COMP_DECAL4,
//	COMP_DECAL5,
//	COMP_DECAL6,
	COMP_TOTAL
};

enum {
	CELL_CUBES_SIZE = 16*16*16*2,
	CELL_DAMAGE_SIZE = 16*16*16,
	CELL_PAINT_SIZE = 16*16*16*6*2,
	CELL_DECAL_SIZE = 16*16*16*6,
};

/*
enum { 
	CDATA_CUBES,
	CDATA_DAMAGE,
	CDATA_PAINT,
	CDATA_DECAL,
	CDATA_TOTAL
};*/
/*
//------------------------------------------------------------------------------------------------------------------------
typedef struct t_RawCell {
	boost::uint16_t	data[16*16*16];
	boost::uint8_t paint[16*16*16*6];
	// todo: user-data list
} RawCell;
*/
//------------------------------------------------------------------------------------------------------------------------
typedef struct t_CompressedCell {
	boost::uint32_t		datasize;
	boost::uint8_t		data[16*16*16*8]; // maximum possible size (no its not?)
} CompressedCell;

struct Cell;

//------------------------------------------------------------------------------------------------------------------------
typedef struct t_CellCompCache {
	boost::uint8_t  damage[16*16*16];
	boost::uint16_t paint[16*16*16*6]; // interleaved in STANDARD face ordering (front,back,right,left,top,bototm
	boost::uint8_t  decal[16*16*16*6]; // interleaved

	Cell *parent;

	struct t_CellCompCache *prev, *next;
} CellCompCache;

//------------------------------------------------------------------------------------------------------------------------
struct Cell {

	__declspec(align(16)) boost::uint16_t cubes[16*16*16];

	Context			*ct;			// parent context of cell
	boost::uint64_t	world_index;	// index of cell in the context
	boost::uint64_t shuffled_index; // index optimized for CellTree operations
	boost::uint32_t	render_flags;	// flags to tell a renderer what is dirty (opaque/translucent cubes removed/added) (see CRF_...)
	boost::uint32_t	owner;			// owner ID of cell
	boost::uint32_t	group;			// group ID of cell
	boost::uint32_t	recorded_size;	// for keeping track of size changes
	//boost::uint8_t	busy;			// cell is being modified in another thread (access with interlocked exchange)		todo: is this going to be used? - notice: not used anymore
	boost::uint8_t	changed;		// data has changed from default and needs to be saved
	boost::uint8_t	empty;			// if this was created from an empty cell state; reset this if any data is created!
	
	
	int				comp_sizes[COMP_TOTAL];	// size of compressed datas 

	boost::uint8_t	*compressed;	// compressed data

	volatile unsigned int codec_cache_state; // for safe operation with codec
	CellCompCache *codec_cache;		// 0 = uncached

	
	Cell			*prev, *next;	// linked list nodes for CellPool

	//Cell			*shadow;		// shadows are used by modification threads
};

//------------------------------------------------------------------------------------------------------------------------
static inline int MaskCubeIndex( int cube ) {
	return cube & 1023;
}

//------------------------------------------------------------------------------------------------------------------------
enum {
	CRF_CUBES_REMOVED	=1,
	CRF_CUBES_ADDED		=2,
	CRF_PAINT_CHANGED	=4,
	CRF_DECAL_CHANGED	=8,
	CRF_CUBES_DAMAGED	=16
};

//------------------------------------------------------------------------------------------------------------------------
void InitializeCellHeader( Cell *ch );
int ComputeCellSize( Cell *ch );

}

//2 Raverz 	With The Crowd (Deamon Remix)
//Rob Mayth 	Barbie Girl 2007 (Manians Freak In da Morning Remix)
