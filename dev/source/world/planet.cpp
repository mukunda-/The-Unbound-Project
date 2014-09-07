//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//

#include "stdafx.h"

#if 0 // replaced !

// planet generation and storage



//-----------------------------------------------------------------------------
namespace planet {
//-----------------------------------------------------------------------------

#define SEED_Y_SCALE 32
#define SEED_Z_SCALE 128

#define SEEDB_Y_SCALE 128
#define SEEDB_Z_SCALE 4096

//#define SEA_LEVEL (PLANET_HEIGHT/2 )
//#define LAND_BASE (PLANET_HEIGHT/2 )

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
typedef struct t_modification {
//-----------------------------------------------------------------------------
	int param[16];
	int type;
	u16 single_source;
	bool finished;
	chunk *source;
	struct t_modification *prev;
	struct t_modification *next;
} modification;

enum {
	MOD_DESTROY_RADIUS,	// x, y, z, r, bitmap
	MOD_DESTROY_RECT, // x, y, z, x2, y2, z2
	MOD_DESTROY_SINGLE, // x, y, z
	
	MOD_COPY_RECT, // x, y, z, 

	MOD_SET_RECT,
	MOD_SET_SINGLE
};

modification *mods_first;
modification *mods_last;

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
u8 collision_map[] = {
//-----------------------------------------------------------------------------
	0,1,1,1, 1,1,1,1, 1,1,1,0, 0,0,0,0,
	1,1,1,1, 0,0,0,0, 0,0,0,0, 0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};

//-----------------------------------
//bit0   1   2   3   4   5   6   7 //
// xyz Xyz xyZ XyZ xYz XYz xYZ XYZ //
//
// (76543210)
//-----------------------------------

typedef struct t_tree_stamp {
	u8 width;
	u8 height;
	u8 depth;

	u8 rootx;
	u8 rooty;
	u8 rootz;

	u8 *data;
} tree_stamp;

tree_stamp *tree_stamps;
int tree_stamp_count;


void tree_leaves( u8 *buffer, int x, int y, int z, int t ) {
	if( (x < 0 || y < 0 || z < 0 || x > 15 || y > 15 || z > 15) ) return;
	if( buffer[x+y*256+z*16] == 0 ) {
		buffer[x+y*256+z*16] = CELL_LEAVES;
		if( t > 1 ) {
			
			
			for( int cx = -1; cx < 2; cx++ ) for( int cy = -1; cy < 1; cy++ ) for( int cz = -1; cz < 2; cz++ ) {
				if( (rand() % 4) == 0 )
					tree_leaves(buffer,x+cx,y+cy,z+cz,t-1);
			}
			
			
		}
	}
	
	
}

void tree_trunk( u8 *buffer, int x, int y, int z, int t, int l ) {
	if( (x < 0 || y < 0 || z < 0 || x > 15 || y > 15 || z > 15) ) return;
	buffer[x+y*256+z*16] = CELL_TREE;

	
	if( t > 1 ) {
		tree_trunk(buffer,x,y+1,z,t-1,l);
	} else {
		
		for( int cx = -1; cx < 2; cx++ ) for( int cy = -1; cy < 2; cy++ ) for( int cz = -1; cz < 2; cz++ ) {
			if( (rand() % 2) != 0 )
				tree_leaves(buffer,x+cx,y+cy,z+cz,l-1);
		}

	}
	
}

void create_tree( tree_stamp *target ) {

	int height = 5 + rand() % 5;
	int size = 3 + rand() % 5;

	u8 work_buffer[16*32*16];

	for( int i =0 ; i < 16*32*16; i++ ) work_buffer[i] = 0;

	tree_trunk( work_buffer, 8, 0, 8, height, size );

	int minx, maxx, miny, maxy, minz, maxz;
	minx = miny = minz = 16;
	maxx = maxy = maxz = -1;

	for( int y = 0; y < 16; y++ ) {
		for( int z = 0; z < 16; z++ ) {
			for( int x = 0; x < 16; x++ ) {
				if( work_buffer[x+y*256+z*16] != 0 ) {
					if( x < minx ) minx = x;
					if( y < miny ) miny = y;
					if( z < minz ) minz = z;

					if( x > maxx ) maxx = x;
					if( y > maxy ) maxy = y;
					if( z > maxz ) maxz = z;
				}
			}
		}
	}

	int sizex, sizey, sizez;
	sizex = 1+maxx-minx;
	sizey = 1+maxy-miny;
	sizez = 1+maxz-minz;

	// SIZES SHOULD AND MUST BE VALID

	target->data = new u8[sizex*sizey*sizez];

	for( int y = 0; y < sizey; y++ ) {
		for( int z = 0; z < sizez; z++ ) {
			for( int x = 0; x < sizex; x++ ) {
				target->data[x+y*(sizex*sizez)+z*(sizex)] = work_buffer[(minx+x)+(miny+y)*256+(minz+z)*16];
			}
		}
	}

	target->width = sizex;
	target->height = sizey;
	target->depth = sizez;
	target->rooty = 0;
	target->rootx = 8 - minx;
	target->rootz = 8 - minz;
	

}

void initialize_tree_stamps() {
	tree_stamp_count = 512;
	tree_stamps = new tree_stamp[tree_stamp_count];

	for( int i = 0; i < tree_stamp_count; i++ ) {
		tree_stamp *target;
		target = tree_stamps + i;
		create_tree(target);
	}
}

void delete_tree_stamps() {
	for( int i = 0; i < tree_stamp_count; i++ ) {
		delete[] tree_stamps[i].data;
	}
	delete tree_stamps;
}

//-----------------------------------------------------------------------------
enum {
//-----------------------------------------------------------------------------
	JOB_REQUEST,		// p1=index
	JOB_GENERATE,		// p1=index
	JOB_SAVE,			// p1=index
	JOB_LOAD,			// p1=index

	SJOB_DIRTY,			// p1=index
//	JOB_SUNLIGHT,		// p1=x, p2=y1, p3=y2, p4=y4		
//	JOB_FULLSUNLIGHT,	// p1=cx, p2=cy, p3=cz				
//	JOB_REMOVELIGHT,	// p1=x, p2=y, p3=z, p4=radius		
//	JOB_ADDLIGHT,		// p1=x, p2=y, p3=z, p4=intensity	
//	JOB_SERVERREQUEST,	// p1=cx, p2=cy, p3=cz				
//	JOB_LIGHTING		// x1,y1,z1,x2,y2,z2				
};

//-----------------------------------------------------------------------------
class t_job_fifo {
//-----------------------------------------------------------------------------
	job *first;
	job *last;

	CRITICAL_SECTION cs;

	
	bool enabled;

public:

	t_job_fifo() {
		
		first=last=0;
		InitializeCriticalSectionAndSpinCount(&cs, 0x00000400);
		enabled=true;
	}

	~t_job_fifo() {
		enabled=false;
		DeleteCriticalSection(&cs);
	}

	void push( job *j ) {
		if( !enabled ) return;

		j->next=0;

		EnterCriticalSection( &cs );

		if( last ) {
			last->next=j;
			last = j;
		} else {
			first=last=j;
		}

		LeaveCriticalSection( &cs );
	}

	job * pop() {
		if( !enabled ) return 0;

		job *j;

		EnterCriticalSection( &cs );

		j = first;

		if( j ) {
			first = first->next;
			if( !first ) last=0;
		}

		LeaveCriticalSection( &cs );

		return j;
	}
} job_fifo, render_jobs ;

//-----------------------------------------------------------------------------
class s_chunk_manager {
//-----------------------------------------------------------------------------
	chunk *first;
	chunk *last;

	int total_chunks; 
	
public:

	//-------------------------------------------------------------------------
	chunk *pop() {
	//-------------------------------------------------------------------------
		// pop new chunk

		if( last->ct ) {
			if( (last->ct->chunks.get( last->table_index )>>8) == last->memory_index ) {
				last->ct->chunks.invalidate( last->table_index );
				last->ct = 0;
			}
			
		}

		//last->instance = 0;

		chunk *poot = last->prev;
		last->prev->next = 0;
		last->prev  = 0;
		first->prev = last;
		last->next  = first;
		first = last;
		last = poot; 

		return first;
	}

	//-------------------------------------------------------------------------
	chunk *bump( chunk* ch ) {
	//-------------------------------------------------------------------------
		if( ch == first ) return ch;
		if( ch == last ) {
			last = last->prev;
			last->next = 0;
		} else {
			ch->next->prev = ch->prev;
			ch->prev->next = ch->next;
		}
		ch->next = first;
		ch->prev = 0;
		first->prev = ch;
		first = ch;
		return ch;
		
	}

	//-------------------------------------------------------------------------
	void push( chunk *ch ) {
	//-------------------------------------------------------------------------
		if( !first ) {
			first = last = ch;
			ch->prev=0;
			ch->next=0;
		} else {
			ch->next = 0;
			ch->prev = last;
			last->next = ch;
			last = ch;
		}
		total_chunks++;
		
	}

	//-------------------------------------------------------------------------
	void prime() {
	//-------------------------------------------------------------------------


#ifdef AB_SERVERSIDE
		int counter = 1000;
		for( chunk *c = last; c != 0 && counter > 0; c = c->prev, counter-- ) {
			if( c->touched ) {
				if( c->unsaved ) {
					save_chunk(c);
					c->unsaved = false;
				}
			}
		}
#endif

		// client side doesnt disk cache
	}

	//-------------------------------------------------------------------------
	void reset_state() {
	//-------------------------------------------------------------------------
		first = 0;
		last = 0;
		total_chunks = 0;
	}

} chunk_manager;
//-----------------------------------------------------------------------------

context *ct_current;
 
void *chunk_memory = 0;
int mem_per_chunk;
 
//bool b_world_loaded = false;
bool enable_disk_cache; // server side (TODO)

HANDLE handleChunkLoader;
DWORD threadChunkLoader;
bool threadChunkLoaderRunning;

#define RND_LENGTH 16384

//-------------------------------------------------------------------------------------------------

//u8 random_table[SEED_LENGTH];
sample3d random_table[SEED_LENGTH+1];

//sample2d random_table[SEED_LENGTH*32];
//sample3d random_table_big[SEED_LENGTH];

//-------------------------------------------------------------------------------------------------
chunk *get_chunk_from_index( int index ) {
//-------------------------------------------------------------------------------------------------
	return (chunk*)((char*)chunk_memory + mem_per_chunk * index);
}

//-------------------------------------------------------------------------------------------------
chunk *to_chunk( CHUNKPTR ch ) {
//-------------------------------------------------------------------------------------------------
	// ch must be valid address
	return get_chunk_from_index( ch >> 8 );
}



//-------------------------------------------------------------------------------------------------
void allocate_chunk_memory( int chunks ) {
//-------------------------------------------------------------------------------------------------
	if( chunk_memory != 0 ) _aligned_free( chunk_memory );
	
	chunk_manager.reset_state();
	
	// pad to 16 bytes
	mem_per_chunk = sizeof(chunk);
	mem_per_chunk += 15;
	mem_per_chunk &= ~15;

	chunk_memory = (void*)_aligned_malloc( mem_per_chunk * chunks, 16 );

	memset( chunk_memory, 0, mem_per_chunk * chunks );

	for( int i = 0; i < chunks; i++ ) {
		chunk_manager.push((chunk*)((char*)chunk_memory + i*mem_per_chunk));
	}

	for( int i = 0; i < chunks; i++ ) {
		chunk *ch = get_chunk_from_index(i);
		ch->memory_index = i;
		ch->ct = 0;
		//ch->instance = 0;
	}
}

//-------------------------------------------------------------------------------------------------
chunktable::chunktable() {
//-------------------------------------------------------------------------------------------------
	valid = false;
	table = 0;
}

//-------------------------------------------------------------------------------------------------
void chunktable::clean() {
//-------------------------------------------------------------------------------------------------
	valid=false;
	if( table ) {
		delete[] table;
	}
}

//-------------------------------------------------------------------------------------------------
chunktable::~chunktable() {
//-------------------------------------------------------------------------------------------------
	clean();
}

//-------------------------------------------------------------------------------------------------
void chunktable::create( int p_width, int p_height, int p_length ) {
//-------------------------------------------------------------------------------------------------

	// CREATE NEW CHUNKTABLE
	// WIDTH,HEIGHT,LENGTH MUST BE POWER OF 2, MINIMUM OF 64
	clean();
	width=p_width;
	height=p_height;
	length=p_length;

	width16 = p_width>>4;
	height16 = p_height>>4;
	length16 = p_length>>4;
	
	table = new CHUNKPTR[width16*height16*length16];
	reset();

	valid = true;
}

//-------------------------------------------------------------------------------------------------
void chunktable::initialize_from_disk( const char *path ) {
//-------------------------------------------------------------------------------------------------
	// todo
}

//-------------------------------------------------------------------------------------------------
void chunktable::reset() {
//-------------------------------------------------------------------------------------------------

	for( int i = 0; i < width16*height16*length16; i++ ) {
		table[i] = CHUNK_INVALID;
	}

}

//-------------------------------------------------------------------------------------------------
int chunktable::index_from_coords( int x, int y, int z ) {
//-------------------------------------------------------------------------------------------------
	return x + y*width16*length16 + z*width16;
}

//-------------------------------------------------------------------------------------------------
void chunktable::coords_from_index( int index, int &x, int &y, int &z ) {
//-------------------------------------------------------------------------------------------------
	x = index & (width16-1);
	z = (index / (width16)) & (length16-1);
	y = (index / (width16*length16));
}

//-------------------------------------------------------------------------------------------------
CHUNKPTR chunktable::set( int index, CHUNKPTR state ) {
//-------------------------------------------------------------------------------------------------
	return InterlockedExchange( table+index, state );
}

//-------------------------------------------------------------------------------------------------
CHUNKPTR chunktable::set( int x, int y, int z, CHUNKPTR state ) {
//-------------------------------------------------------------------------------------------------
	return set( index_from_coords(x,y,z), state );
}

//-------------------------------------------------------------------------------------------------
CHUNKPTR chunktable::invalidate( int index ) {
//-------------------------------------------------------------------------------------------------
	return set( index, CHUNK_INVALID );
}

//-------------------------------------------------------------------------------------------------
CHUNKPTR chunktable::invalidate( int x, int y, int z ) {
//-------------------------------------------------------------------------------------------------
	return set( x, y, z, CHUNK_INVALID );
}

//-------------------------------------------------------------------------------------------------
CHUNKPTR chunktable::get( int x, int y, int z ) {
//-------------------------------------------------------------------------------------------------
	if( x < 0 || y < 0 || z < 0 || x >= width16 || y >= height16 || z >= length16 ) return CHUNK_EMPTY;
	return table[index_from_coords(x,y,z)];
}

//-------------------------------------------------------------------------------------------------
CHUNKPTR chunktable::get( int index ) {
//-------------------------------------------------------------------------------------------------
	return table[index];
}

//-------------------------------------------------------------------------------------------------
CHUNKPTR * chunktable::get_pointer( int x, int y, int z ) {
//-------------------------------------------------------------------------------------------------
	return &table[index_from_coords(x,y,z)];
}



//-------------------------------------------------------------------------------------------------
context::context() {
//-------------------------------------------------------------------------------------------------
	//skylight_registry = 0;
	//highest_point=0;
	//lowest_point=0;
	//sunlight_dirty_low = 0;
	//sunlight_dirty_high = 0;

	InitializeCriticalSectionAndSpinCount( &cs_regs, 0x400 );
}

//-------------------------------------------------------------------------------------------------
context::~context() {
//-------------------------------------------------------------------------------------------------
	//if( skylight_registry ) delete[] skylight_registry;
	/*if( highest_point ) delete[] highest_point;
	if( lowest_point ) delete[] lowest_point;
	if( sunlight_dirty_low ) delete[] sunlight_dirty_low;
	if( sunlight_dirty_high ) delete[] sunlight_dirty_high;
	*/
	DeleteCriticalSection( &cs_regs );
}

//-------------------------------------------------------------------------------------------------
void context::initialize( const char *mapfile, const char *regionfiles ) {
//-------------------------------------------------------------------------------------------------

	
	datapath = regionfiles;
	map.load( mapfile );

	width = map.width * 16;
	depth = map.depth * 16;
	length = map.length * 16;

	width16 = map.width ;
	depth16 = map.depth ;
	length16 = map.length ;

	sea_level = map.sea_level;
	
	//skylight_registry = new u16[width16*length16];

	//highest_point = new u8[width16*length16];
	//lowest_point = new u8[width16*length16];
	//sunlight_dirty_low = new u8[width16*length16];
	//sunlight_dirty_high = new u8[width16*length16];

//	for( int i = 0; i < width16*length16; i++ ) {
//		skylight_registry[i] = 0;
		/*sunlight_dirty_high[i] = 0;
		sunlight_dirty_low[i] = height16-1;
		highest_point[i] = 0;
		lowest_point[i] = height16-1;*/
//	}

	chunks.create(width,depth,length);
}
/*
//-------------------------------------------------------------------------------------------------
bool context::is_sunlight_dirty( int x, int z ) {
//-------------------------------------------------------------------------------------------------
	if( skylight_registry[x+z*width16] > 0 ) {
		return worldrender::
	}
	return sunlight_dirty_high[x+z*width16] >= sunlight_dirty_low[x+z*width16];
}

//-------------------------------------------------------------------------------------------------
void context::register_height( int x, int y, int z ) {
//-------------------------------------------------------------------------------------------------
	EnterCriticalSection( &cs_regs );

	if( highest_point[x+z*width16] < y ) highest_point[x+z*width16] = y;
	if( lowest_point[x+z*width16] > y ) lowest_point[x+z*width16] = y;
	
	LeaveCriticalSection( &cs_regs );
}
*/
/*
//-------------------------------------------------------------------------------------------------
void context::register_sunlight( int x, int y, int z ) {
//-------------------------------------------------------------------------------------------------
	EnterCriticalSection( &cs_regs );

	if( sunlight_dirty_low[x+z*width16] > y ) sunlight_dirty_low[x+z*width16] = y;
	if( sunlight_dirty_high[x+z*width16] < y ) sunlight_dirty_high[x+z*width16] = y;

	LeaveCriticalSection( &cs_regs );
}

//-------------------------------------------------------------------------------------------------
void context::register_height_sunlight( int x, int y, int z ) {
//-------------------------------------------------------------------------------------------------
	EnterCriticalSection( &cs_regs );

	if( highest_point[x+z*width16] < y ) highest_point[x+z*width16] = y;
	if( lowest_point[x+z*width16] > y ) lowest_point[x+z*width16] = y;

	if( sunlight_dirty_low[x+z*width16] > y ) sunlight_dirty_low[x+z*width16] = y;
	if( sunlight_dirty_high[x+z*width16] < y ) sunlight_dirty_high[x+z*width16] = y;
	
	LeaveCriticalSection( &cs_regs );
}
*/
/*
//-------------------------------------------------------------------------------------------------
void context::copy_state( int x, int z, int *op_highest_point, int *op_lowest_point, int *op_sunlight_high, int *op_sunlight_low ) {
//-------------------------------------------------------------------------------------------------
	EnterCriticalSection( &cs_regs );

	if( op_highest_point ) *op_highest_point = highest_point[x+z*width16];
	if( op_lowest_point ) *op_lowest_point = lowest_point[x+z*width16];
	if( op_sunlight_high ) *op_sunlight_high = sunlight_dirty_high[x+z*width16];
	if( op_sunlight_low ) *op_sunlight_low = sunlight_dirty_low[x+z*width16];

	LeaveCriticalSection( &cs_regs );
}

//-------------------------------------------------------------------------------------------------
void context::copy_and_reset_sunlight( int x, int z, int *op_highest_point, int *op_lowest_point, int *op_sunlight_high, int *op_sunlight_low ) {
//-------------------------------------------------------------------------------------------------
	EnterCriticalSection( &cs_regs );

	if( op_highest_point ) *op_highest_point = highest_point[x+z*width16];
	if( op_lowest_point ) *op_lowest_point = lowest_point[x+z*width16];
	if( op_sunlight_high ) *op_sunlight_high = sunlight_dirty_high[x+z*width16];
	if( op_sunlight_low ) *op_sunlight_low = sunlight_dirty_low[x+z*width16];

	sunlight_dirty_high[x+z*width16] = 0;
	sunlight_dirty_low[x+z*width16] = height-1;

	LeaveCriticalSection( &cs_regs );
}
*/

#ifdef AB_SERVERSIDE
//-------------------------------------------------------------------------------------------------
void store_chunk( chunk* ch ) {
//-------------------------------------------------------------------------------------------------
	chunktable *ct = ch->table;
	if( real_chunk(ct->get(ch->table_index)) ) {
		if( enable_disk_cache ) {
			if( ch->touched ) {
				
			}
		}
	}
	ct->set( ch->table_index, 
}


//-------------------------------------------------------------------------------------------------
chunk *load_chunk( int table_index ) {
//-------------------------------------------------------------------------------------------------
	
	chunktable *ct;
	
	CHUNK_STATE cs = ct_current->get(table_index);
	chunk *newchunk;
	if( real_chunk(cs) ) {
		store_chunk(to_chunk(cs));
		newchunk = to_chunk(cs);
		chunk_cache.bump(newchunk);
	} else {
		newchunk = chunk_cache.pop();
	}
	reset_chunk(newchunk);
	return newchunk;
}
#endif

/*
//-------------------------------------------------------------------------------------------------
chunk *alloc_chunk( u16 world_index ) {
//-------------------------------------------------------------------------------------------------
	chunk *ch;
	
	for( int i = 7; i >= 0; i-- ) {
		ch = free_chunk_list[i].pop();
		if(ch) break;
	}
	
	if( ch ) {
		delete_chunk(ch);
		used_chunk_list.push(ch);
	}
	
	return ch;
}

//-------------------------------------------------------------------------------------------------
void cache_chunk( chunk *c, int cache_level ) {
//-------------------------------------------------------------------------------------------------
	used_chunk_list.pop(c);
	free_chunk_list[cache_level].push(c);
}

//-------------------------------------------------------------------------------------------------
void delete_chunk( chunk *c ) {
//-------------------------------------------------------------------------------------------------
	used_chunk_list.pop(c);

}
*/

//=================================================================================================
//=================================================================================================
//
//SAMPLING FUNCTIONS
//
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------

const int cosinterp2[] = {
0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 4, 4, 5, 5, 6, 6, 6, 7, 7, 8, 9, 9, 10, 10, 11, 12, 12, 13, 14, 14, 15, 16, 17, 17, 18, 19, 20, 21, 22, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 
37, 39, 40, 41, 42, 43, 44, 46, 47, 48, 49, 50, 52, 53, 54, 56, 57, 58, 60, 61, 62, 64, 65, 66, 68, 69, 70, 72, 73, 75, 76, 78, 79, 80, 82, 83, 85, 86, 88, 89, 91, 92, 94, 95, 97, 98, 100, 101, 103, 105, 106, 108, 109, 111, 112, 114, 115, 117, 119, 120, 122, 123, 125, 126, 
128, 130, 131, 133, 134, 136, 137, 139, 141, 142, 144, 145, 147, 148, 150, 151, 153, 155, 156, 158, 159, 161, 162, 164, 165, 167, 168, 170, 171, 173, 174, 176, 177, 178, 180, 181, 183, 184, 186, 187, 188, 190, 191, 192, 194, 195, 196, 198, 199, 200, 202, 203, 204, 206, 207, 208, 209, 210, 212, 213, 214, 215, 216, 217, 
219, 220, 221, 222, 223, 224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 234, 235, 236, 237, 238, 239, 239, 240, 241, 242, 242, 243, 244, 244, 245, 246, 246, 247, 247, 248, 249, 249, 250, 250, 250, 251, 251, 252, 252, 253, 253, 253, 254, 254, 254, 254, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, };

const int costab[] = {

65536, 65535, 65531, 65525, 65516, 65505, 65492, 65476, 65457, 65436, 65413, 65387, 65358, 65328, 65294, 65259, 65220, 65180, 65137, 65091, 65043, 64993, 64940, 64884, 64827, 64766, 64704, 64639, 64571, 64501, 64429, 64354, 64277, 64197, 64115, 64031, 63944, 63854, 63763, 63668, 63572, 63473, 63372, 63268, 63162, 63054, 62943, 62830, 62714, 62596, 62476, 62353, 62228, 62101, 61971, 61839, 61705, 61568, 61429, 61288, 61145, 60999, 60851, 60700, 
60547, 60392, 60235, 60075, 59914, 59750, 59583, 59415, 59244, 59071, 58896, 58718, 58538, 58356, 58172, 57986, 57798, 57607, 57414, 57219, 57022, 56823, 56621, 56418, 56212, 56004, 55794, 55582, 55368, 55152, 54934, 54714, 54491, 54267, 54040, 53812, 53581, 53349, 53114, 52878, 52639, 52398, 52156, 51911, 51665, 51417, 51166, 50914, 50660, 50404, 50146, 49886, 49624, 49361, 49095, 48828, 48559, 48288, 48015, 47741, 47464, 47186, 46906, 46624, 
46341, 46056, 45769, 45480, 45190, 44898, 44604, 44308, 44011, 43713, 43412, 43110, 42806, 42501, 42194, 41886, 41576, 41264, 40951, 40636, 40320, 40002, 39683, 39362, 39040, 38716, 38391, 38064, 37736, 37407, 37076, 36744, 36410, 36075, 35738, 35401, 35062, 34721, 34380, 34037, 33692, 33347, 33000, 32652, 32303, 31952, 31600, 31248, 30893, 30538, 30182, 29824, 29466, 29106, 28745, 28383, 28020, 27656, 27291, 26925, 26558, 26190, 25821, 25451, 
25080, 24708, 24335, 23961, 23586, 23210, 22834, 22457, 22078, 21699, 21320, 20939, 20557, 20175, 19792, 19409, 19024, 18639, 18253, 17867, 17479, 17091, 16703, 16314, 15924, 15534, 15143, 14751, 14359, 13966, 13573, 13180, 12785, 12391, 11996, 11600, 11204, 10808, 10411, 10014, 9616, 9218, 8820, 8421, 8022, 7623, 7224, 6824, 6424, 6023, 5623, 5222, 4821, 4420, 4019, 3617, 3216, 2814, 2412, 2010, 1608, 1206, 804, 402, };


__forceinline int sample_trilinear_cosine( int x, int y, int z, int offset ) {
	
	// compute table index
	int xi = (x>>8), yi = (y>>8), zi = (z>>8);
	int index = (offset + xi + yi * SEED_Y_SCALE + zi * SEED_Z_SCALE) & (SEED_MASK);

	u8 xf = x&0xFF, yf = y&0xFF, zf = z&0xFF; // x,y,z fractions
	//u8 xf = cosinterp2[x&0xFF], yf = cosinterp2[y&0xFF], zf = cosinterp2[z&0xFF]; // x,y,z fractions

	// compute a,b,c factor pairs (f),(1-f)
	int a = ((zf^255))|(zf<<8);
	int b = ((16384-(yf<<6)) )|((yf<<6)<<16);
	int c = ((256-(xf)))|((xf)<<16);
	//int b = ((yf^255)<<6)|((yf<<6)<<16);
	//int c = ((xf^255))|((xf<<16));

	sample3d *s = random_table + index;
	__m128i data =  _mm_loadl_epi64( (__m128i *)s );// data = random sample data
													// data = 8 bytes, .8 precision

	__m128i factors = _mm_cvtsi32_si128( a );		// factors = [~z][z] (2 bytes)
	factors = _mm_shufflelo_epi16( factors, 0x00 );	// factors = [~z][z][~z][z][~z][z][~z][z] (8 bytes)
	data = _mm_maddubs_epi16( factors, data );		// multiply vertically and add
													// data = 4 words, .16 precision

	factors = _mm_cvtsi32_si128( b );				// factors = [~y][y] (2 words)
	factors = _mm_shufflelo_epi16( factors, 0x44 );	// factors = [~y][y][~y][y] (4 words)
	data = _mm_mulhi_epi16( data, factors );		// multiply vertically and save hiword
													// data = 4 words, .14 precision

	factors = _mm_cvtsi32_si128( c );				// factors = [~z][z] (2 words)	
	factors = _mm_shufflelo_epi16( factors, 0x50 );	// factors = [~z][z][~z][z] (4 words)
	data = _mm_madd_epi16( data, factors );			// multiply vertically and add
													// data = 4 words, .22 precision
	data = _mm_hadd_epi32( data, data );			// add products together
													// data = 2 words, .22 precision
	int result = _mm_cvtsi128_si32( data ) >> 6;	// return shifted result (.16 precision)
	return result;
}

//----------------------------------------------------------------------------------------------------------------

__m128i shuffle_maskA = _mm_set_epi8( 5,4,5,4,5,4,5,4,5,4,5,4,5,4,5,4 );
__m128i shuffle_maskB = _mm_set_epi8( (char)128,3, (char)128,2, (char)128,3, (char)128,2, (char)128,3, (char)128,2, (char)128,3, (char)128,2 );
__m128i shuffle_maskC = _mm_set_epi8( (char)128,(char)128,(char)128,(char)1,  (char)128,(char)128,(char)128,(char)0,  (char)128,(char)128,(char)128,(char)1,  (char)128,(char)128,(char)128,(char)0 );
__m128i xor_mask = _mm_set_epi32(0x00000000,0x00000000,0x00FF00FF,0x00FF00FF);

//----------------------------------------------------------------------------------------------------------------
// compute 2d sample from 3d data with trilinear interpolation
//----------------------------------------------------------------------------------------------------------------
__forceinline void sample2_trilinear_cosine( u32 x1, u32 y1, u32 z1, int offset1, int offset2, int &result1, int &result2 ) { 
//----------------------------------------------------------------------------------------------------------------

	// split fractions and integers
	// compute table indices
	int xi1 = x1 >> 8, yi1 = y1 >> 8, zi1 = z1 >> 8;
	int index1 = (xi1 + yi1 * SEED_Y_SCALE + zi1 * SEED_Z_SCALE);// & (random_mask);
	int index2 = (index1 + offset2) & (SEED_MASK);
	index1 = (index1 + offset1) & (SEED_MASK);

	int xf1 = x1 & 255, yf1 = y1 & 255, zf1 = z1 & 255;
	//int a = (xf1)|(yf1<<8)|(zf1<<16);
	int a = (cosinterp2[xf1])|(cosinterp2[yf1]<<8)|(cosinterp2[zf1]<<16);
	
	__m128i factors1 = _mm_cvtsi32_si128(a);			// [0][z][y][x]
	factors1 = _mm_unpacklo_epi8( factors1, factors1 ); // [0][0][z][z][y][y][x][x]
	factors1 = _mm_xor_si128( factors1, xor_mask );		
	// factors1: [0][FF][z][~z][y][~y][x][~x]
	//			  7   6  5   4  3   2  1   0
	
	// data = [data2][data1] 16 bytes
	sample3d *s1 = random_table + index1;
	__m128i data =  _mm_loadl_epi64( (__m128i *)s1 );
	sample3d *s2 = random_table + index2;
	data =  _mm_castpd_si128(_mm_loadh_pd( _mm_castsi128_pd(data), (double *)s2 ));
	//----------------------------------------------------------------------------
	// Z AXIS:
	// factors = [z][~z][z][~z][z][~z][z][~z][z][~z][z][~z][z][~z][z][~z]
	__m128i factors = _mm_shuffle_epi8( factors1, shuffle_maskA );
	data = _mm_maddubs_epi16( factors, data );			// multiply and add words
														// data is now 4+4 words (.16 precision)
	//----------------------------------------------------------------------------
	// Y AXIS:
	factors = _mm_shuffle_epi8( factors1, shuffle_maskB );// factors = [y][~y][y][~y][y][~y][y][~y] (words)
	data = _mm_madd_epi16( data, factors );				// multiply and add words
														// data is now 2+2 dwords (.24 precision)
	//----------------------------------------------------------------------------
	// X AXIS:
	factors = _mm_shuffle_epi8( factors1, shuffle_maskC );// factors = [x][~x][x][~x] (dwords)
	data = _mm_mullo_epi32( data, factors );			// multiply by factors
														// data is now 2+2 dwords (.32 precision)
	data = _mm_hadd_epi32( data, data );				// add adjacent samples
	//----------------------------------------------------------------------------
	data = _mm_srai_epi32( data, 16 );					// reduce samples (32->16)
	result1 = (_mm_cvtsi128_si32( data ));				// data[63:32] = sample2
	result2 = (_mm_extract_epi32( data, 1 ));			// data[31:0]  = sample1
	return;
}

//----------------------------------------------------------------------------------------------
__forceinline int sample_bilinear_cosine( int x, int y, int offset ) {
//----------------------------------------------------------------------------------------------
	int index = (x>>8)+(y>>8)*SEED_Y_SCALE+offset;
	sample3d *samp = random_table + index;

	x &= 255; y &= 255;
	x = cosinterp[x]; 
	y = cosinterp[y];
	                                                                 
	int a = (samp[0].data[0]<<8) + (samp[1].data[0] - samp[0].data[0]) * x; // 16 bit results
	int b = (samp[0].data[2]<<8) + (samp[1].data[2] - samp[0].data[2]) * x;
	a = (a<<8) + (b-a) * y; // 24bit result
	return (a) >> 8; // no rounding? return 16bit unsigned result
}

//----------------------------------------------------------------------------------------------
__forceinline int sample_bilinear( int x, int y, int offset ) {
//----------------------------------------------------------------------------------------------
	int index = (x>>8)+(y>>8)*SEED_Y_SCALE+offset;
	sample3d *samp = random_table + index;

	x &= 255; y &= 255;
	                                                                 
	int a = (samp[0].data[0]<<8) + (samp[1].data[0] - samp[0].data[0]) * x; // 16 bit results
	int b = (samp[0].data[2]<<8) + (samp[1].data[2] - samp[0].data[2]) * x;
	a = (a<<8) + (b-a) * y; // 24bit result
	return (a) >> 8; // no rounding? return 16bit unsigned result
}

/*
//-------------------------------------------------------------------------------------------------
double sample_2d_cosine_hq( int x, int y, int offset ) {
//-------------------------------------------------------------------------------------------------
	
    int x2 = (x+65536);
	int y2 = (y+65536);

	double a = random_table[(offset+(x>>16)+(y>>16)*191) & SEED_MASK];
	double b = random_table[(offset+(x2>>16)+(y>>16)*191) & SEED_MASK];
	double c = random_table[(offset+(x>>16)+(y2>>16)*191) & SEED_MASK];
	double d = random_table[(offset+(x2>>16)+(y2>>16)*191) & SEED_MASK];

	x = (32768 - cos_table[(x&65535)>>6]) >> 1;
	y = (32768 - cos_table[(y&65535)>>6]) >> 1;
	double dx = (double)(x) / 32768.0;
	double dy = (double)(y) / 32768.0;
	
	a += ((b-a)*dx);
	c += ((d-c)*dx);
	return (a + ((c-a)*dy));
}*/
/*
//-------------------------------------------------------------------------------------------------
int sample_3d_linear_plerp( int ux, int y, int z ) {
//-------------------------------------------------------------------------------------------------
	ux>>=3;
	int xi,yi,zi;
	int xf;
	yi = y>>8;
	zi = z>>8;
	xi = ux>>5;
	xf = ux&31;
	
	sample2d *data = &random_table_3d[((xi+yi*SEED_Y_SCALE+zi*SEED_Z_SCALE)&SEED_MASK)*32+xf];
	y &= 255;
	z &= 255;

	int a = (data->a<<8)+(((data->b-data->a) * y));
	int c = (data->c<<8)+(((data->d-data->c) * y));
	return ((a<<8) + ((c-a)*z)+32768)>>16;
}*/

/*
//-------------------------------------------------------------------------------------------------
int sample_3d_cosine_plerp( int ux, int y, int z ) {
//-------------------------------------------------------------------------------------------------
	ux>>=3;
	int xi,yi,zi;
	int xf;
	yi = y>>8;
	zi = z>>8;
	xi = ux>>5;
	xf = ux&31;
	
	sample2d *data = &random_table_3d[((xi+yi*SEED_Y_SCALE+zi*SEED_Z_SCALE)&SEED_MASK)*32+xf];
	y &= 255;
	z &= 255;

	int a = (data->a<<8)+(((data->b-data->a) * (cosinterp[y])));
	int c = (data->c<<8)+(((data->d-data->c) * (cosinterp[y])));
	a >>= 2;
	c >>= 2;
	return ((a<<8) + ((c-a)*(cosinterp[z]))+32768)>>14;
}*/
/*
//-------------------------------------------------------------------------------------------------
int sample_2d_cosine_ft( int x, int y, int offset ) {
//-------------------------------------------------------------------------------------------------
	

	sample2d *data = &random_table_3d[(((x>>8)*SEED_Y_SCALE+(y>>8)*SEED_Z_SCALE+offset)&SEED_MASK)*32];

	int a = (data->a<<8)+((data->b-data->a) * cosinterp[(x&255)]);
	int c = (data->c<<8)+((data->d-data->c) * cosinterp[(x&255)]);
	a = ((a<<8) + (c-a)*cosinterp[(y&255)] + 32768) >> 16;
	return a;
}*/
/*
//-------------------------------------------------------------------------------------------------
int sample_2d_cosine_ft_n( int x, int y, int offset ) {
//-------------------------------------------------------------------------------------------------

	sample2d *data = &random_table_3d[(((x>>8)*SEED_Y_SCALE+(y>>8)*SEED_Z_SCALE+offset)&SEED_MASK)*32];

	int a = (data->a<<8)+((data->b-data->a) * cosinterp[(x&255)]);
	int c = (data->c<<8)+((data->d-data->c) * cosinterp[(x&255)]);
	a = ((a<<8) + (c-a)*cosinterp[(y&255)] + 32768) >> 16;
	return a;
}*/
/*
//-------------------------------------------------------------------------------------------------
int sample_3d_big_hq( int px, int py, int pz, int offset ) {
//-------------------------------------------------------------------------------------------------
	sample3d *data = &random_table_big[(px>>8)+(py>>8)*SEEDB_Y_SCALE+(pz>>8)*SEEDB_Z_SCALE+offset];
	px&=255;
	py&=255;
	pz&=255;

	// samples = 0.8

	// interpolate X
	int yz = ((data[0].xyz<<8)+(data[1].xyz-data[0].xyz)*cosinterp[px]);
	int Yz = ((data[0].xYz<<8)+(data[1].xYz-data[0].xYz)*cosinterp[px]);
	int yZ = ((data[0].xyZ<<8)+(data[1].xyZ-data[0].xyZ)*cosinterp[px]);
	int YZ = ((data[0].xYZ<<8)+(data[1].xYZ-data[0].xYZ)*cosinterp[px]);

	// results = 0.16

	// interpolate Y
	int z = ((yz<<8)+(Yz-yz)*cosinterp[py]+2)>>2;
	int Z = ((yZ<<8)+(YZ-yZ)*cosinterp[py]+2)>>2;

	// results = 0.22

	// interpolate z

	int r = (z<<8)+(Z-z)*cosinterp[pz];

	// final result = 0.16
	return r >> 14;

}*/

//-------------------------------------------------------------------------------------------------
int sample_rnd( int x, int y, int offset ) {
//-------------------------------------------------------------------------------------------------

	sample3d *data = &random_table [((x>>8)*SEED_Y_SCALE+(y>>8)*SEED_Z_SCALE+offset)&SEED_MASK ];

	return data->data[0];
}

/*
int sample_1d_cosine ( int x, int offset ) {
	int a = random_table [(((x>>8)+offset)&SEED_MASK)];
	int b = random_table [(((x>>8)+offset+1)&SEED_MASK)];
	a = (a<<8)+((b-a) * cosinterp[(x&255)]);
	a = (a+128)>>8;
	return a;
}*/

/*
//-------------------------------------------------------------------------------------------------
double perlin_2d_cosine_hq( int offset, int scale, int x, int y, int depth ) {
//-------------------------------------------------------------------------------------------------
	float result=0;
	for( int d = 1; d <= depth; d++ ) {
		result += (float)sample_2d_cosine_hq( x*scale*d*2, y*scale*d*2,   offset + d*4 ) / (float)(1<<d);
	}
	return result;
}*/


//=================================================================================================
//=================================================================================================

//-------------------------------------------------------------------------------------------------
chunk *create_empty_chunk( int x, int y, int z ) {
//-------------------------------------------------------------------------------------------------

	if( read_state(ct_current->chunks.get(x,y,z)) == CHUNK_EMPTY ) {
		chunk *c = chunk_manager.pop(); 
		if( !c ) return 0;
		for( int i = 0; i < 16*16*16; i++ ) {
			c->data[i] = CELL_AIR;
			//c->sunlight[i] = 0;
		}
	

		//c->instance=0;
		c->touched=0;
		///c->solid=0;
		//c->empty=1;
		//c->dimlight_changed=1;
		//c->sunlight_changed=1;
		//c->dirty=1;
		c->modified=0;

		ct_current->chunks.set(x,y,z,CHUNK_READY | (c->memory_index<<8));
		//planet_data[ x | (y*PLANET_WIDTH16*PLANET_WIDTH16) | (z*PLANET_WIDTH16) ] = CHUNK_REAL+c->index;

		///ct_current->register_height_sunlight( x,y,z );
		
		//ct_current->register_chunk_height( x, y, z ); // todo: thread safe!
		//ct_current->register_sunlight_modification( x, y, z );
		return c;
	} else {
		// errornous path, chunk should be known to be empty
		return 0;
	}
}
   
void setup_random_table( int seed ) {
	rnd::reset(seed);

	for( int i = 0; i < SEED_LENGTH; i++ ) {
		random_table[i].data[0] = (s8)((double)rnd::next_double() * (double)255 - 128.0); // todo, may not equal 255 ever?
	}

	for( int i = 0; i < SEED_LENGTH; i++ ) {
		random_table[i].data[1] = random_table[(i + SEED_Z_SCALE)&SEED_MASK].data[0];
		random_table[i].data[2] = random_table[(i + SEED_Y_SCALE)&SEED_MASK].data[0];
		random_table[i].data[3] = random_table[(i + SEED_Z_SCALE+SEED_Y_SCALE)&SEED_MASK].data[0];
	}

	random_table[SEED_LENGTH] = random_table[0];
	/*
	for( int i = 0; i < SEED_LENGTH; i++ ) {
		for( int x = 0; x < 32; x++ ) {
			int a,b,base1,base2,delta1,delta2;
 
			a=random_table[i];				// 0,0
			b=random_table[(i+1)&SEED_MASK];	//
			base1 = (a*256 + (b-a)*cosinterp[x*8])/256;

			a=random_table[(i+SEED_Z_SCALE)&SEED_MASK];		// +z
			b=random_table[(i+1+SEED_Z_SCALE)&SEED_MASK];
			base2 = (a*256 + (b-a)*cosinterp[x*8])/256;

			a=random_table[(i+SEED_Y_SCALE)&SEED_MASK];		// +y
			b=random_table[(i+1+SEED_Y_SCALE)&SEED_MASK];	//
			delta1 = (a*256 + (b-a)*cosinterp[x*8])/256;

			a=random_table[(i+SEED_Y_SCALE+SEED_Z_SCALE)&SEED_MASK];	 // +z,+y
			b=random_table[(i+1+SEED_Y_SCALE+SEED_Z_SCALE)&SEED_MASK];
			delta2 = (a*256 + (b-a)*cosinterp[x*8])/256;
			
			random_table_3d[i*32+x].a = base1;
			random_table_3d[i*32+x].b = delta1;
			random_table_3d[i*32+x].c = base2;
			random_table_3d[i*32+x].d = delta2;
		}
	}*/

	/*
	for( int i = 0; i < SEED_LENGTH; i++ ) {
		random_table_big[i].xyz = (u8)((double)rnd::next_double() * (double)255);
	}*/

	/*
	for( int i = 0; i < SEED_LENGTH; i++ ) {
		random_table_big[i].xYz = random_table_big[i+(SEEDB_Y_SCALE)].xyz;
		random_table_big[i].xyZ = random_table_big[i+(SEEDB_Z_SCALE)].xyz;
		random_table_big[i].xYZ = random_table_big[i+(SEEDB_Y_SCALE+SEEDB_Z_SCALE)].xyz;
	}
	*/
	 
}

/*
void create( const char *map_file ) { // TODO
	//world=p;

	setup_random_table( 32798 );
	
	BinaryFile f;
	if( f.open( map_file, BinaryFile::MODE_READ ) ) {

		f.seek( 512 );

		for( int i = 0; i < 1024*1024; i++ ) {
			planet_map[i].elevation = f.read8();
			planet_map[i].turba = f.read8();
			planet_map[i].turbf = f.read8();
			planet_map[i].density = f.read8();
			planet_map[i].mountains = f.read8();
			planet_map[i].trees = f.read8();
			planet_map[i].temperature = f.read8();
			planet_map[i].rainfall = f.read8();
			planet_map[i].saltwater = f.read8();
			planet_map[i].fog = f.read8();
			planet_map[i].cave1 = f.read8();
			planet_map[i].cave2 = f.read8();
			planet_map[i].cave3 = f.read8();
			planet_map[i].cave4 = f.read8();
			planet_map[i].interpolation = f.read8();
			f.read8();
		}

		f.close();

	} else {
		// error
	}

	
	//FILE *f = fopen(map_file, "rb"); {
		
	//} fclose(f);

	for( int i = 0; i < PLANET_WIDTH16*PLANET_WIDTH16*PLANET_HEIGHT16; i++ ) {
		planet_data[i] = CHUNK_UNKNOWN;
	}
	//TODO load from region files

	for( int x = 0; x < PLANET_WIDTH16; x++ ) {
		for( int z = 0; z < PLANET_WIDTH16; z++ ) {
			sunlight_dirty_high[x+z*PLANET_WIDTH16] = 0;
			sunlight_dirty_low[x+z*PLANET_WIDTH16] = PLANET_HEIGHT16-1;
			highest_point[x+z*PLANET_WIDTH16] = 0;
			lowest_point[x+z*PLANET_WIDTH16] = PLANET_HEIGHT16-1;
		}
	}


	b_world_loaded = true;
}
*/

//=================================================================================================
//=================================================================================================
/*
void dirty_instance( chunk *c, bool geometry, bool dimlight, bool skylight ) {
	if( c->instance ) {
		worldrender::dirty_instance( c->instance-1, geometry, dimlight, skylight );
	}
}

void dirty_instance( CHUNKPTR p, bool geometry, bool dimlight, bool skylight ) {
	if( real_chunk(p) ) {
		dirty_instance( to_chunk(p), geometry, dimlight, skylight );
	}
}

void dirty_sides( int x, int y, int z, bool geometry, bool dimlight, bool skylight ) {

	for( int cx = x-1; cx <= x+1; cx++ ) {
		for( int cy = y-1; cy <= y+1; cy++ ) {
			for( int cz = z-1; cz <= z+1; cz++ ) {
				if( cx < 0 || cy < 0 || cz < 0 ) continue;
				// todo, upper bounds
				CHUNKPTR ix;
				//if( ct_current->wrapping_enabled ) {
				//	ix = get_chunk_pointer_wrapped(cx,cy,cz);
				//} else {
					ix = get_chunk_pointer(cx,cy,cz);
				//}
				dirty_instance( ix, geometry, dimlight, skylight );
				
			}
		}
	}
}
*/
void add_dirty_job( int x, int y, int z, bool geometry, bool dimlight, bool skylight ) {
	job *j = new job();
	j->index = ct_current->chunks.index_from_coords(x,y,z);
	j->ct = ct_current;
	j->ch = 0;
	j->type = SJOB_DIRTY;
	j->data1 = (geometry ? 1 : 0) | (dimlight ? 2 : 0) | (skylight ? 4 : 0);
	render_jobs.push(j);
}

void do_render_jobs() {
	job *j;
	context *ct;
	int x,y,z;

	while( j = render_jobs.pop() ) {
		ct = j->ct;
		
		switch( j->type )  {
		case SJOB_DIRTY:
			ct->chunks.coords_from_index( j->index, x,y,z );
			worldrender::dirty_area( x,y,z, !!(j->data1 & 1), !!(j->data1 & 2), !!(j->data1 & 4) );
			if( j->data1 & 4 ) {
				worldrender::set_skylight_dirty( ct, x, y, z );
			}
			//dirty_sides( x,y,z, j->data1 & 1, j->data1 & 2, j->data1 & 4 );
		}
	}
}

/*
_CRT_ALIGN(16) u8 sunlight_stamp[16*16];

_CRT_ALIGN(16) u32 mask1[4] = {0x0f0f0f0f, 0x0f0f0f0f, 0x0f0f0f0f, 0x0f0f0f0f};
_CRT_ALIGN(16) u32 sub2[4] = {0x0c0c0c0c, 0x0c0c0c0c, 0x0c0c0c0c, 0x0c0c0c0c};

void recalc_sunlight( int x, int z ) {

	CHUNKPTR chp;
	chunk *ch;

	u8 *data1;
	u8 *data2;

	int start, end, hp, lp;

	ct_current->copy_and_reset_sunlight( x, z, &hp, &lp, &start, &end );

	//hp = hp * 16 + 15;
	lp = lp * 16;
	start = start * 16 + 15;
	end = end * 16;

	//int start = sunlight_dirty_high[ x |  (z*PLANET_WIDTH16) ] * 16+15;
	//int end = sunlight_dirty_low[ x |  (z*PLANET_WIDTH16) ]*16;
	
	//int hp = highest_point[ x |  (z*PLANET_WIDTH16) ];
	

	//if( (start>>4) != hp )  {
	
	int foundstamp =0;
	u32 default_stamp = 0x0f0f0f0f;

	for( int y = (start>>4)+1; y <= hp; y++ ) {
		chp = get_chunk_pointer(x,y,z);
		//chp = ct_current->chunks.get(x,y,z);// planet_data[ (x) | (y*PLANET_WIDTH16*PLANET_WIDTH16) | ((z)*PLANET_WIDTH16) ];

		// if passing through an invalid chunk, set default stamp to 0 (for cave environments)
		if( read_state(chp) == CHUNK_INVALID )
			default_stamp = 0;


		if( real_chunk(chp) ) {
			foundstamp=1;
			ch = to_chunk(chp);
			for(int i = 0; i < 256/4; i++ )
				((unsigned int*)sunlight_stamp)[i] = ((unsigned int*)ch->sunlight)[i];
			break;
		}
	}
	//}

	if( !foundstamp ) {
		for(int i = 0; i < 256/4; i++ )
			((unsigned int*)sunlight_stamp)[i] = default_stamp;
	}

	for( int y = start; y >= lp; y-- ) {
		if( (y&15) == 15 ) {
			
			//ch = world->data[ x | ((y>>4)*PLANET_WIDTH16*PLANET_WIDTH16) | (z*PLANET_WIDTH16) ];
			ch = get_chunk( get_chunk_pointer(x,y>>4,z) );// get_chunk_real( x, y>>4, z );
			if( ch ) {
				data1 =  ch->data +15*256;
				data2 =  ch->sunlight +15*256;
				//ch->light_dirty=1;
				//ch->dirty=1;
				//ch->sunlight_changed=1;
				//dirty_instance(ch,false,false,true);
				dirty_sides(x,y>>4,z,false,false,true);
				//chunk *ch2 = world->data[ ((x-1)&(PLANET_WIDTH16-1)) | ((y>>4)*PLANET_WIDTH16*PLANET_WIDTH16) | (z*PLANET_WIDTH16) ];
				//if( valid_chunk_addr(ch2) ) ch2->light_dirty=1;
				//  ch2 = world->data[ ((x+1)&(PLANET_WIDTH16-1)) | ((y>>4)*PLANET_WIDTH16*PLANET_WIDTH16) | (z*PLANET_WIDTH16) ];
				//if( valid_chunk_addr(ch2) ) ch2->light_dirty=1;
				 //  ch2 = world->data[ ((x)) | ((y>>4)*PLANET_WIDTH16*PLANET_WIDTH16) | (((z-1)&(PLANET_WIDTH16-1))*PLANET_WIDTH16) ];
				//if( valid_chunk_addr(ch2) ) ch2->light_dirty=1;
				// ch2 = world->data[ ((x)) | ((y>>4)*PLANET_WIDTH16*PLANET_WIDTH16) | (((z+1)&(PLANET_WIDTH16-1))*PLANET_WIDTH16) ];
				//if( valid_chunk_addr(ch2) ) ch2->light_dirty=1;
			} else {

				bool skip=true;
				//chunk *ch2 = world->data[ ((x-1)&(PLANET_WIDTH16-1)) | ((y>>4)*PLANET_WIDTH16*PLANET_WIDTH16) | (z*PLANET_WIDTH16) ];
				//if( valid_chunk_addr(ch2) ) if( !ch2->empty ) skip=false;
				//ch2 = world->data[ ((x+1)&(PLANET_WIDTH16-1)) | ((y>>4)*PLANET_WIDTH16*PLANET_WIDTH16) | (z*PLANET_WIDTH16) ];
				//if( valid_chunk_addr(ch2) ) if( !ch2->empty ) skip=false;
				//ch2 = world->data[ ((x)) | ((y>>4)*PLANET_WIDTH16*PLANET_WIDTH16) | (((z-1)&(PLANET_WIDTH16-1))*PLANET_WIDTH16) ];
				//if( valid_chunk_addr(ch2) ) if( !ch2->empty ) skip=false;
				//ch2 = world->data[ ((x)) | ((y>>4)*PLANET_WIDTH16*PLANET_WIDTH16) | (((z+1)&(PLANET_WIDTH16-1))*PLANET_WIDTH16) ];
				//if( valid_chunk_addr(ch2) ) if( !ch2->empty ) skip=false;
			//	chunk *ch2;
			//	if( y > 15 ) {
			//		ch2 = world->data[ ((x)) | (((y>>4)-1)*PLANET_WIDTH16*PLANET_WIDTH16) | (((z)&(PLANET_WIDTH16-1))*PLANET_WIDTH16) ];
			//		if( valid_chunk_addr(ch2) ) if( !ch2->empty ) skip=false;
			//	}

			//	if( skip ) {
			//		
					y &= ~15;
					continue;
				
			//	} else {
			//		if( ch == CHUNK_EMPTY ) {
			//			
			//			
			//			ch = create_empty_chunk(x,y>>4,z);
			//			
			//			data1 =  ch->data +15*256;
			//			data2 =  ch->sunlight +15*256;
			//			ch->light_dirty=1;
			//			ch->dirty=1;
			//			ch->sunlight_dirty=0;
			//			
			//			chunk *ch2 = world->data[ ((x-1)&(PLANET_WIDTH16-1)) | ((y>>4)*PLANET_WIDTH16*PLANET_WIDTH16) | (z*PLANET_WIDTH16) ];
			//			if( valid_chunk_addr(ch2) ) ch2->light_dirty=1;
			//			  ch2 = world->data[ ((x+1)&(PLANET_WIDTH16-1)) | ((y>>4)*PLANET_WIDTH16*PLANET_WIDTH16) | (z*PLANET_WIDTH16) ];
			//			if( valid_chunk_addr(ch2) ) ch2->light_dirty=1;
			//			   ch2 = world->data[ ((x)) | ((y>>4)*PLANET_WIDTH16*PLANET_WIDTH16) | (((z-1)&(PLANET_WIDTH16-1))*PLANET_WIDTH16) ];
			//			if( valid_chunk_addr(ch2) ) ch2->light_dirty=1;
			//			 ch2 = world->data[ ((x)) | ((y>>4)*PLANET_WIDTH16*PLANET_WIDTH16) | (((z+1)&(PLANET_WIDTH16-1))*PLANET_WIDTH16) ];
			//			if( valid_chunk_addr(ch2) ) ch2->light_dirty=1;
						
			//		} else {
			//			y &= ~15;
			//			continue;
			//		}
			//	}
			}
		}
		
		if( y < end ) {
			//if( !(ch->sunlight_invalid_bitmap & (1<<(y&15))) ) {
				bool stoproutine=true;
				for(int i = 0; i < 256/4; i++ ) {
					if( ((unsigned int*)sunlight_stamp)[i] != ((unsigned int*)data2)[i] ) {
						stoproutine=false;
						break; 
					}
				}
				if( stoproutine ) break;
			//}
		}

		//ch->sunlight_invalid_bitmap &= ~(1<<(y&15));

		__asm {
			//mov		ecx, 4
			mov		eax, [data1]
			mov		edx, [data2]
			//mov		ebx, 0

			movdqa	xmm4, mask1
			movdqa	xmm5, sub2

loop1:

			//---------------------------------------------------------------------------
			movdqa	xmm0, [eax]
			movdqa	xmm1, [eax+0x10]
			movdqa	xmm2, [eax+0x20]
			movdqa	xmm3, [eax+0x30]
			movdqa	xmm6, sunlight_stamp[0]
			movdqa	xmm7, sunlight_stamp[0x10]
			pand	xmm0, xmm4
			pand	xmm1, xmm4
			pand	xmm2, xmm4
			pand	xmm3, xmm4
			psubb	xmm0, xmm5
			psubb	xmm1, xmm5
			psubb	xmm2, xmm5
			psubb	xmm3, xmm5
			
			psubusb	xmm6, xmm0
			psubusb	xmm7, xmm1
			movdqa	[edx], xmm6
			movdqa	[edx+0x10], xmm7
			movdqa	sunlight_stamp[0], xmm6
			movdqa	sunlight_stamp[0x10], xmm7

			movdqa	xmm6, sunlight_stamp[0x20]
			movdqa	xmm7, sunlight_stamp[0x30]
			psubusb	xmm6, xmm2
			psubusb	xmm7, xmm3
			movdqa	[edx+0x20], xmm6
			movdqa	[edx+0x30], xmm7
			movdqa	sunlight_stamp[0x20], xmm6
			movdqa	sunlight_stamp[0x30], xmm7

			//---------------------------------------------------------------------------
			movdqa	xmm0, [eax+0x40]
			movdqa	xmm1, [eax+0x50]
			movdqa	xmm2, [eax+0x60]
			movdqa	xmm3, [eax+0x70]
			movdqa	xmm6, sunlight_stamp[0x40]
			movdqa	xmm7, sunlight_stamp[0x50]
			pand	xmm0, xmm4
			pand	xmm1, xmm4
			pand	xmm2, xmm4
			pand	xmm3, xmm4
			psubb	xmm0, xmm5
			psubb	xmm1, xmm5
			psubb	xmm2, xmm5
			psubb	xmm3, xmm5

			
			
			psubusb	xmm6, xmm0
			psubusb	xmm7, xmm1
			movdqa	[edx+0x40], xmm6
			movdqa	[edx+0x50], xmm7
			movdqa	sunlight_stamp[0x40], xmm6
			movdqa	sunlight_stamp[0x50], xmm7

			movdqa	xmm6, sunlight_stamp[0x60]
			movdqa	xmm7, sunlight_stamp[0x70]
			psubusb	xmm6, xmm2
			psubusb	xmm7, xmm3
			movdqa	[edx+0x60], xmm6
			movdqa	[edx+0x70], xmm7
			movdqa	sunlight_stamp[0x60], xmm6
			movdqa	sunlight_stamp[0x70], xmm7

			//---------------------------------------------------------------------------
			movdqa	xmm0, [eax+0x80]
			movdqa	xmm1, [eax+0x90]
			movdqa	xmm2, [eax+0xa0]
			movdqa	xmm3, [eax+0xb0]
			movdqa	xmm6, sunlight_stamp[0x80]
			movdqa	xmm7, sunlight_stamp[0x90]
			pand	xmm0, xmm4
			pand	xmm1, xmm4
			pand	xmm2, xmm4
			pand	xmm3, xmm4
			psubb	xmm0, xmm5
			psubb	xmm1, xmm5
			psubb	xmm2, xmm5
			psubb	xmm3, xmm5
			psubusb	xmm6, xmm0
			psubusb	xmm7, xmm1

			movdqa	[edx+0x80], xmm6
			movdqa	[edx+0x90], xmm7
			movdqa	sunlight_stamp[0x80], xmm6
			movdqa	sunlight_stamp[0x90], xmm7

			movdqa	xmm6, sunlight_stamp[0xa0]
			movdqa	xmm7, sunlight_stamp[0xb0]
			psubusb	xmm6, xmm2
			psubusb	xmm7, xmm3
			movdqa	[edx+0xa0], xmm6
			movdqa	[edx+0xb0], xmm7
			movdqa	sunlight_stamp[0xa0], xmm6
			movdqa	sunlight_stamp[0xb0], xmm7

			//---------------------------------------------------------------------------
			movdqa	xmm0, [eax+0xc0]
			movdqa	xmm1, [eax+0xd0]
			movdqa	xmm2, [eax+0xe0]
			movdqa	xmm3, [eax+0xf0]
			movdqa	xmm6, sunlight_stamp[0xc0]
			movdqa	xmm7, sunlight_stamp[0xd0]
			pand	xmm0, xmm4
			pand	xmm1, xmm4
			pand	xmm2, xmm4
			pand	xmm3, xmm4
			psubb	xmm0, xmm5
			psubb	xmm1, xmm5
			psubb	xmm2, xmm5
			psubb	xmm3, xmm5

			psubusb	xmm6, xmm0
			psubusb	xmm7, xmm1
			movdqa	[edx+0xc0], xmm6
			movdqa	[edx+0xd0], xmm7
			movdqa	sunlight_stamp[0xc0], xmm6
			movdqa	sunlight_stamp[0xd0], xmm7

			movdqa	xmm6, sunlight_stamp[0xe0]
			movdqa	xmm7, sunlight_stamp[0xf0]
			psubusb	xmm6, xmm2
			psubusb	xmm7, xmm3
			movdqa	[edx+0xe0], xmm6
			movdqa	[edx+0xf0], xmm7
			movdqa	sunlight_stamp[0xe0], xmm6
			movdqa	sunlight_stamp[0xf0], xmm7

		//	add		ebx, 64
		//	dec		ecx
		//	jne		loop1
			
		}

	//	for( int cz = 0; cz <= 240; cz += 16 ) {
	//		
	//		data += 16;
	//	}
		data1 -= 256;
		data2 -= 256;
	}
	
//	sunlight_dirty_low[x+z*PLANET_WIDTH16] = PLANET_HEIGHT-1;
//	sunlight_dirty_high[x+z*PLANET_WIDTH16] = 0;
}

*/
/*
void set_chunk( int index, PLANET_INDEX ch ) {
	
	if( planet_data[index] != ch ) {


		if( real_chunk(planet_data[index]) ) {
			
			chunk *oc = to_chunk(planet_data[index]);

#ifndef AB_SERVERSIDE
			if( real_chunk(ch) ) {
				chunk *c = to_chunk(ch);
				c->instance = oc->instance;
				c->sunlight_changed=1;
				c->dimlight_changed=1;
				//c->sunlight_spread=0;
			} else {

				if( oc->instance ) {
					worldrender::reset_instance(oc->instance-1);
				}

			}
#endif
			free_chunk(oc);//delete world->data[index];
		}

		planet_data[index] = ch;

		int x = index & (PLANET_WIDTH16-1);
		int z = (index>>PLANET_WIDTH_BITS) & (PLANET_WIDTH16-1);
		int y = (index>>(PLANET_WIDTH_BITS*2));
		dirty_sides(x,y,z);

	}
	

	
}
*/

void test_solid_faces( chunk *ch ) {
	return;
	/*
	ch->solid=CHUNK_SOLID_ALL;
	
	// LEFT
	for( int cy = 0; cy < 16 && (ch->solid&CHUNK_SOLID_LEFT); cy++ ) {
		for( int cz = 0; cz < 16; cz++ ) {
			if( (ch->data[cy*256+cz*16] &0xf) >= 12 ) {
				ch->solid ^= CHUNK_SOLID_LEFT;
				break;
	}}}


	// RIGHT
	for( int cy = 0; cy < 16 && (ch->solid&CHUNK_SOLID_RIGHT); cy++ ) {
		for( int cz = 0; cz < 16; cz++ ) {
			if( (ch->data[15+cy*256+cz*16]  &0xf) >= 12 ) {
				ch->solid ^= CHUNK_SOLID_RIGHT;
				break;
	}}}

	// FRONT
	for( int cy = 0; cy < 16 && (ch->solid&CHUNK_SOLID_FRONT); cy++ ) {
		for( int cx = 0; cx < 16; cx++ ) {
			if( (ch->data[cx+(cy)*256+(15)*16]  &0xf) >= 12 ) {
				ch->solid ^= CHUNK_SOLID_FRONT;
				break;
	}}}

	// BACK
	for( int cy = 0; cy < 16 && (ch->solid&CHUNK_SOLID_BACK); cy++ ) {
		for( int cx = 0; cx < 16; cx++ ) {
			if( (ch->data[cx+(cy)*256]  &0xf) >= 12 ) {
				ch->solid ^= CHUNK_SOLID_BACK;
				break;
	}}}

	// TOP
	for( int cz = 0; cz < 16 && (ch->solid&CHUNK_SOLID_TOP); cz++ ) {
		for( int cx = 0; cx < 16; cx++ ) {
			if( (ch->data[cx+(cz)*16+15*256]  &0xf) >= 12 ) {
				ch->solid ^= CHUNK_SOLID_TOP;
				break;
	}}}
	
	// BOTTOM
	for( int cz = 0; cz < 16 && (ch->solid&CHUNK_SOLID_BOTTOM); cz++ ) {
		for( int cx = 0; cx < 16; cx++ ) {
			if(( ch->data[cx+(cz)*16]  &0xf) >= 12 ) {
				ch->solid ^= CHUNK_SOLID_BOTTOM;
				break;
	}}}
	*/
}

namespace WORKTHREAD {
//// NON MAIN THREAD

template <class pooda>
void interpolate_maps_x16( int type, pooda *result, pooda *source, int sourcewidth ) {

	const int max_width = 8;

	int sw1 = sourcewidth+1;
	int sw16 = sourcewidth*16;

	if( type == 0 ) {
		// NO INTERPOLATION
		for( int x = 0; x < sourcewidth; x++ ) {
			for( int y = 0; y < sourcewidth; y++ ) {
				for( int x2 = 0; x2 < 16; x2++ ) {
					for( int y2 = 0; y2 < 16; y2++ ) {
						result[x*16+(y*16)*sw16+x2+(y2*sw16)] = source[x+y*sw1];
					}
				}
			}
		}
	} else if( type == 1 ) {
		int buffer[(max_width*16)*(max_width+1)]; // work buffer x.4 fixed
		int *dest = buffer;
		// interpolate Y
		for( int x = 0; x < sw1; x++ ) {
			
			for( int i = 0; i < sourcewidth; i++ ) {

				
				int a, b;
				a = source[i*sw1+x];
				b = source[i*sw1+x+sw1];
				for( int y = 0; y < 16; y++ ) {
					int mu = y<<4;
					*dest++ = (a * (256-mu) + b * mu);
				}
			}
		}

		// interpolate X
		for( int y = 0; y < sw16; y++ ) {
			
			for( int i = 0; i < sourcewidth; i++ ) {
				int a, b;
				a = buffer[y + i * sw16];
				b = buffer[y + (i+1) * sw16];
				for( int x = 0; x < 16; x++ ) {
					int mu = x<<4;
					*result++ = (a * (256-mu) + b * mu + 32768) >> 16;
				}
			}
		}
	} else if( type == 2 ) {

		// COPY OF ABOVE WITH CHANGED INTERPOLATION FUNCTION

		int buffer[(max_width*16)*(max_width+1)]; // work buffer x.4 fixed
		int *dest = buffer;
		// interpolate Y
		for( int x = 0; x < sw1; x++ ) {
			
			for( int i = 0; i < sourcewidth; i++ ) {

				
				int a, b;
				a = source[i*sw1+x];
				b = source[i*sw1+x+sw1];
				for( int y = 0; y < 16; y++ ) {
					
					int mu = cosinterp[y<<4];
					*dest++ = (a * (256-mu) + b * mu);
				}
			}
		}

		// interpolate X
		for( int y = 0; y < sw16; y++ ) {
			
			for( int i = 0; i < sourcewidth; i++ ) {
				int a, b;
				a = buffer[y + i * sw16];
				b = buffer[y + (i+1) * sw16];
				for( int x = 0; x < 16; x++ ) {
					
					int mu = cosinterp[x<<4];
					*result++ = (a * (256-mu) + b * mu + 32768) >> 16;
				}
			}
		}
	}
}
/*
void interpolate3x3_48x48( int type, u8 *result, u8 *source  ) {
	// type = none,linear,cosine
	// result=  48x48 bytes
	// source = 4x4 bytes
	if( type == 0 ) {
		// NO INTERPOLATION
		for( int x = 0; x < 3; x++ ) {
			for( int y = 0; y < 3; y++ ) {
				for( int x2 = 0; x2 < 16; x2++ ) {
					for( int y2 = 0; y2 < 16; y2++ ) {
						result[x*16+(y*16)*48+x2+(y2*48)] = source[x+y*4];
					}
				}
			}
		} 
	} else if( type == 1 ) {
		int buffer[48*4]; // work buffer x.4 fixed
		int *dest = buffer;
		// interpolate Y
		for( int x = 0; x < 4; x++ ) {
			
			for( int i = 0; i < 3; i++ ) {

				
				int a, b;
				a = source[i*4+x];
				b = source[i*4+x+4];
				for( int y = 0; y < 16; y++ ) {
					int mu = y<<4;

					*dest++ = (a * (256-mu) + b * mu);
				}
			}
		}

		// interpolate X
		for( int y = 0; y < 48; y++ ) {
			
			for( int i = 0; i < 3; i++ ) {
				int a, b;
				a = buffer[y + i * 48];
				b = buffer[y + (i+1) * 48];
				for( int x = 0; x < 16; x++ ) {
					int mu = x<<4;
					*result++ = (a * (256-mu) + b * mu) >> 16;
				}
			}
		}
	} else if( type == 2 ) {
		int buffer[48*4]; // work buffer x.4 fixed
		int *dest = buffer;
		// interpolate Y
		for( int x = 0; x < 4; x++ ) {
			
			for( int i = 0; i < 3; i++ ) {

				
				int a, b;
				a = source[i*4+x];
				b = source[i*4+x+4];
				for( int y = 0; y < 16; y++ ) {
					int mu = cosinterp[y<<4];
					*dest++ = (a * (256-mu) + b * mu);
				}
			}
		}

		// interpolate X
		for( int y = 0; y < 48; y++ ) {
			
			for( int i = 0; i < 3; i++ ) {
				int a, b;
				a = buffer[y + i * 48];
				b = buffer[y + (i+1) * 48];
				for( int x = 0; x < 16; x++ ) {
					int mu = cosinterp[x<<4];
					*result++ = (a * (256-mu) + b * mu+32768) >> 16;
				}
			}
		}
	}
}*/

void plot( chunk *ch, int x, int y, int z, int c ) {
	if( x >= 0 && x < 16 && y >= 0 && y < 16 && z >= 0 && z < 16 ) {
		if( ch->data[x+y*256+z*16] == CELL_AIR )
			ch->data[x+y*256+z*16] = c;
	}
}

void plot_disc( chunk *ch, int x, int y, int z, int w, int c ) {
	int r = w / 2;
	for( int cx = 0; cx < w; cx++ ) {
		for( int cy = 0; cy < w; cy++ ) {
			//if( (cx-r)*(cx-r) + (cy-r)*(cy-r) < r*r ) {
				plot(ch,x-r+cx,y,z-r+cy,c);
			//}
		}
	}
}

void plant_tree( chunk *ch, int x, int y, int z, int seed ) {
	/*
	plot(ch,x,y+1,z,CELL_TREE);
	plot(ch,x,y+2,z,CELL_TREE);
	plot(ch,x,y+3,z,CELL_TREE);
	plot(ch,x,y+4,z,CELL_TREE);
	
	plot_disc( ch,x,y+4,z,3,CELL_LEAVES);
	plot_disc( ch,x,y+5,z,5,CELL_LEAVES);
	plot_disc( ch,x,y+6,z,5,CELL_LEAVES);
	plot_disc( ch,x,y+7,z,5,CELL_LEAVES);
	plot_disc( ch,x,y+8,z,3,CELL_LEAVES);
	*/

	seed = seed & (tree_stamp_count-1);

	tree_stamp *t = tree_stamps + seed;

	x -= t->rootx;
	y -= t->rooty;
	z -= t->rootz;
	y++;

	int tx, ty, tz;
	
	for( int cy = 0; cy < t->height; cy++ ) {
		ty = y + cy;
		if( ty < 0 ) continue;
		if( ty > 15 ) break;
		for( int cz = 0; cz < t->depth; cz++ ) {
			tz = z + cz;
			if( tz < 0 ) continue;
			if( tz > 15 ) break;
			for( int cx = 0; cx < t->width; cx++ ) {
				tx = x + cx;
				if( tx < 0 ) continue;
				if( tx > 15 ) break;

				int b = t->data[cx+cy*(t->width*t->depth)+cz*(t->width)];

				if( b ) {
					if( ch->data[tx+ty*256+tz*16] == CELL_AIR ) {
						ch->data[tx+ty*256+tz*16] = b;
					}
				} 
			}
		}
	}
}

__forceinline int lerp2( int a, int b, int c, int d, int x, int y ) {
	a = (a<<8) + (b-a) * x;
	c = (c<<8) + (d-c) * x;
	a = (a<<8) + (c-a) * y;
	return a >> 8;
}

// input (x.0), (x.0), 8bit interpolator
// returns (x.8 fp) result
__forceinline int lerp1( int a, int b, int x ) {
	return (a<<8) + (b-a) * x;
}

/*
void get_density_caves( map_param *mp, int y, int sea_level, int &r_density, int &r_caves ) {
	if( !mp ) {
		r_density = r_caves = 0;
		return;
	}

	// sea_level>>2 : 256
	int slm = 65536 / (sea_level>>2);
	int a, b, c;
	// TODO: interpolate between parameters if nasty :)
	if( y >= sea_level ) {
		a = b = mp->params[MP_CAVE1];
		c = 0;
		
	} else if( y >= ((sea_level * 3) >> 2) ) { // SEA_LEVEL * 3/4
		a = mp->params[MP_CAVE2];
		b = mp->params[MP_CAVE1];
		c = y - ((sea_level*3)>>2);
		
		
	} else if( y >= ((sea_level) >> 1) ) {	// ty >= SEA_LEVEL * 2/4
		a = mp->params[MP_CAVE3];
		b = mp->params[MP_CAVE2];
		c = y - ((sea_level>>1));
		
	} else if( y >= (sea_level >> 2) ) {
		a = mp->params[MP_CAVE4];
		b = mp->params[MP_CAVE3];
		c = y - ((sea_level>>2));

	} else {
		a = 0;
		b = mp->params[MP_CAVE4];
		c = y;
		
	}
	c = (c * slm) >> 8;
	
	r_density = lerp1( a>>4, b>>4, c );
	r_caves = lerp1( a>>4, b>>4, c );
}*/

void generate_chunk_c1_plains() {

}

void generate_chunk( job *j ) {

	chunk *ch = j->ch;
	unsigned int SEA_LEVEL = j->ct->sea_level;
	
	
	
	////// GENERATION CODE HERE //////

	int x,y,z;
	j->ct->chunks.coords_from_index( j->index, x,y,z );
	world_parameters *map = &j->ct->map;

	// copy zone forms
	u8 zones[48*48];
	for( int cz = -1; cz <= 1; cz++ ) {
		for( int cx = -1; cx <= 1; cx++ ) {
			if( (x+cx) < 0 || (x+cx) >= map->width || (z+cz) < 0 || (z+cz) >= map->length ) {
				for( int i = 0; i < 16*16; i++ ) {
					zones[(cx*16)+(cz*16*48) + (i&15) + (i>>4)*48] = 0;
				}
			} else {
				const map_param_h *source = map->data_h + x + cx + (z + cz) * map->width;
				for( int i = 0; i < 16*16; i++ ) {
					zones[(cx*16)+(cz*16*48) + (i&15) + (i>>4)*48] = source[i].params[MPH_ZONES];
				}
			}
		}
	}

	//for( int 

	//u8 form[16*16*32/8]; // x=0-15, y=-1 to 16, z = 0-15

	u8 output[16*16*18];

	
	for( int i = 0; i < 16*16*18; i++ ) {
		output[i] = CELL_AIR;
	} 

#if 0
	 
	short tmountainmap[24*24];
	u8 mountainshades[24*24];

	u8 forestation[4*4];
	 
	u8 map_density[27];
	u8 map_caves[27];

	int density_threshold;
	int caves_threshold;
	int rainfall;

	int turbulence;

	u8 heightsource[8*8];
	
	{
		u8 mountainsource[8*8];
		for( int hx = 0; hx < 8; hx++ )  {
			for( int hz = 0; hz < 8; hz++ ) {
				map_param *mp = j->ct->map.read_data(x-3+hx,z-3+hz);
				
				heightsource[hx+hz*8] = mp ? mp->params[MP_ELEVATION] : 0;
				mountainsource[hx+hz*8] = mp ? mp->params[MP_MOUNTAINS] : 0;
				
			}
		}
 		//interpolate5x5_80x80( INTERPOLATION_NONE, heightmap, heightsource );
		// Todo: interpolation types
		//interpolate_maps_x16( INTERPOLATION_LINEAR, heightmap, heightsource, 5 );

		//interpolate_maps_x16( INTERPOLATION_LINEAR, mountainmap, mountainsource, 5 );

		/*  REAL TURBULENCE CODE HERE
		for( int hz = 0; hz < 56; hz++ ) { // 0-20 to 15+20
			for( int hx = 0; hx < 56; hx++ ) {
				int turbx, turbz;
				turbx = sample_bilinear_cosine((x*16+hx)*20,(z*16+hz)*20 , 0 )+32768;
				turbz = sample_bilinear_cosine((x*16+hx)*20,(z*16+hz)*20 , 489 )+32768;
				turbx = ((turbx * 16) >> 16) - 8; // Todo: apply turbulence parameter
				turbz = ((turbz * 16) >> 16) - 8;
				//turbx>>=2;
				//turbz>>=2;
				//turbx=turbz=0;
				theightmap[hx+hz*56] = heightmap[(12+hx+turbx)+(12+hz+turbz)*80];
			}
		}*/

		map_param *mp = j->ct->map.read_data(x,z);

		rainfall = mp ? mp->params[MP_RAINFALL] : 0; // controls grass/dirt/sand
		turbulence = mp ? mp->params[MP_TURBA] : 0;
		//255 - (mp ? mp->params[MP_DENSITY] : 0);
		//for( int i = 0; i < 4; i++ ) {
		//	int a = mp ? mp->params[MP_CAVE1+i] : 0;
		//	cave_p[i] = a&15;
		//	cave_d[i] = a>>4;
		//}

		//density_threshold = caves_threshold = 0;
		//if( mp ) {
		//	int ty = y*16+16;
		//////////////////////
		//}
		
		
		for( int mx = 0; mx < 4; mx++ ) {
			for( int mz = 0; mz < 4; mz++ ) {
				map_param *mp = j->ct->map.read_data(x-1+mx,z-1+mz);
				forestation[mx+mz*4] = mp ? mp->params[MP_TREES] : 0;
			}
		}



		for( int hz = 0; hz < 24; hz++ ) {
			for( int hx = 0; hx < 24; hx++ ) {
				//  perlin mountains

				int tx = -640 + hx * 64; // 1/4 chunk per unit
				int ty = -640 + hz * 64;
				u8 *mountainsource_pointer = &mountainsource[(3+(tx>>8))+(3+(ty>>8))*8];
				int mountains = lerp2( mountainsource_pointer[0], mountainsource_pointer[1], mountainsource_pointer[8], mountainsource_pointer[9], tx&255, ty&255 ) >> 8;

				//u8 *heightsource_pointer = &heightsource[(3+(tx>>8))+(3+(ty>>8))*8];
				//int height = lerp2( heightsource_pointer[0], heightsource_pointer[1], heightsource_pointer[8], heightsource_pointer[9], tx&255, ty&255 );

				//int m = mountainmap[(hx+12)+(hz+12)*80];// mountains[(hx>>4)+(hz>>4)*3];
				
				
				if( mountains ) {

					mountains = 256-mountains;
					mountains = mountains*mountains >> 8;
					mountains = 256-mountains;
					
					int tx, tz;
					tx = x*16 - 40 + hx * 4;
					tz = z*16 - 40 + hz * 4;
					
					int mfreq = 3;

					// get max value of 3 samples, low persistence

					int b = (sample_bilinear_cosine( (tx)*mfreq , (tz)*mfreq ,   0 ));
						b+= (sample_bilinear_cosine( (tx)*mfreq*2, (tz)*mfreq*2,   0 )) >> 2;

					int c;
					c = (sample_bilinear_cosine( (tx)*mfreq+111 , (tz)*mfreq ,   111 ));
					c+= (sample_bilinear_cosine( (tx)*mfreq*2, (tz)*mfreq*2,   222 )) >> 2;
					if( c > b ) b = c;
					
					c = (sample_bilinear_cosine( (tx)*mfreq+41 , (tz)*mfreq+88 ,   333 )) ;//* 256;
					c+= (sample_bilinear_cosine( (tx)*mfreq*2, (tz)*mfreq*2+166,   444 )) >> 2;
					if( c > b ) b = c;

					b = b + 32768 + 8192; // unsign and scale data
					b = ((b*400) + 32768) >> 16;
					
					// compute mountain shade
					int ms = b;
					ms = ms - 300;
					ms = ms * 4 / 256;//(600-150); 
					
					if( ms < 0 ) ms = 0;
					if( ms > 3 ) ms = 3;
					ms = 3-ms;
					mountainshades[hx+hz*24] = ms;
					
					tmountainmap[hx+hz*24] = (b*mountains)>>8;
					
				} else {
					tmountainmap[hx+hz*24] = 0;
					
				}
			}
		}

		// translate height
		/*
		for( int i = 0; i < 56*56; i++ ) {
			if( theightmap[i] < tmountainmap[i] )
				theightmap[i] = tmountainmap[i];
		}*/
	}

	//int elevation = j->ct->map.read_data(x,z)->params[MP_ELEVATION];
	//elevation = elevation - 128 + SEA_LEVEL;
	int pd = 200; // density scale

#define DENSITY_TEST (sample_3d_linear_plerp((tx)*pd/32 + 555, (ty)*pd/32, (tz)*pd/32))
	
	u8 *target = output;//ch->data+(cx+cy*256+cz*16);
	int tx, ty, tz;
	
	ty = y*16 - 1;
	for( int cy = -1; cy <= 16; cy++ ) { 
		tz = z*16;
		for( int cz = 0; cz < 16; cz++ ) { 
			tx = x*16;
			for( int cx = 0; cx < 16; cx++ ) {
				
				int p = 2;

		//		tx = x*16+cx;
		//		ty = y*16+cy;
		//		tz = z*16+cz;

				int s1, s2, s3, s4;
				sample2_trilinear_cosine( tx*8, ty*8, tz*8, 0, 55, s1, s2 );
				sample2_trilinear_cosine( tx*16, ty*16, tz*16, 152, 333, s3, s4 );
				// s1,s2,s3,s4 16 bit signed results
				
				// noise samples, 2 octaves, 0.5 persistence
				// NORMALIZE SAMPLES:
				int turbx = ((s1*2+s3) * 85) >> 8; // (S1*2+S3*1)/3
				int turbz = ((s2*2+s4) * 85) >> 8;
				
				// turbx,z = normalized 16bit samples (-32767,32767)

				//turbx = turbx * 0.5;
				//turbz = turbz * 0.5;
				

				// units in heightmap are spaced 16 units apart (256 = 16 units)
				int heightsource_x = (3*256) + (cx*16+((turbx*480)>>15)); // turb * 30 / 16 * 256 / (16bit)
				int heightsource_y = (3*256) + (cz*16+((turbz*480)>>15));
				u8 *heightsource_pointer = heightsource + (heightsource_x>>8) + (heightsource_y>>8)*8;
				int original_elevation = lerp2( 
					heightsource_pointer[0],
					heightsource_pointer[1],
					heightsource_pointer[8], 
					heightsource_pointer[9], heightsource_x & 0xFF, heightsource_y & 0xFF );
				

				// units in mountainmap are spaced 4 units apart
				heightsource_x = (10*256) + (cx*64  + ((turbx*500)>>15)); // turb * 30/4 * 256 / (16bit)
				heightsource_y = (10*256) + (cz*64  + ((turbz*500)>>15));
				short *mountainsource_pointer = &tmountainmap[(heightsource_x>>8)+(heightsource_y>>8)*24];
				int mountain_elevation = lerp2(
					mountainsource_pointer[0],
					mountainsource_pointer[1],
					mountainsource_pointer[24],
					mountainsource_pointer[25],
					heightsource_x & 0xFF,
					heightsource_y & 0xFF );
				
				int elevation = original_elevation + mountain_elevation;
				original_elevation = (original_elevation)>>8;
				original_elevation -= 128;
				original_elevation += SEA_LEVEL;
				//if( ty <= elevation>>8 ) {
				//	elevation += turbc ;
				//}
				elevation = (elevation + 128) >> 8;

				bool hole = false;
				
				elevation = elevation - 128 + SEA_LEVEL;
				

				if( ty <= elevation ) {

					if( ty >= SEA_LEVEL || ty < elevation - 8 ) {  // no holes under the ocean

						//bool hole = false;
						//density = 255-(cave_dy*16);
						/*
						if( density != 255 ) {

							//int density_threshold = elevation - ty;
							//density_threshold -= 32;

							//if( density_threshold > 0 ) density_threshold = 0;
							//density_threshold = -density_threshold;

							
						//	int a = DENSITY_TEST;
							int a = (sample_3d_linear_plerp((tx*1)*pd/32 + 222, (ty*1)*pd/32, (tz*1)*pd/32));
							int b = (sample_3d_linear_plerp((tx*1)*pd/32 + 2553, (ty*1)*pd/32, (tz*1)*pd/32));
							if( b > a ) a = b;
							//int c = (sample_3d_linear_plerp((tx*4+cx)*pd*2 + 8481, (ty*4+cy)*pd*2, (tz*4+cz)*pd*2));
							//if( c > 150 ) a =0 ;;
							//a += (sample_3d_linear_plerp((x*16+cx)*pd*2 + 222, (y*16+cy)*pd*4, (z*16+cz)*p*2))>>1;
							//a += (sample_3d_linear_plerp((x*16+cx)*pd*4 + 0, (y*16+cy)*pd*8, (z*16+cz)*p*4))>>2;
							//a = (a * density_threshold * 146) >> 16;
							//a = (a * density_threshold) >> 5;
							//if( a > density ) hole = true;
							if( a > 175 ) hole = true;
						}
						*/
						if( !hole ) {

							int a, b;
							int a1,b1;
							int a2,b2;
							int cave_py = 5;
							heightsource_x = (tx  + ((turbx*16)>>15)); // turb * 30/4 * 256 / (16bit)
							heightsource_y = (tz  + ((turbz*16)>>15));

							sample2_trilinear_cosine(heightsource_x*1*cave_py , ty*1*cave_py, heightsource_y*1*cave_py, 12, 9979, a, b );// - 128;
							sample2_trilinear_cosine(heightsource_x*2*cave_py , ty*2*cave_py, heightsource_y*2*cave_py, 555, 2521, a1, b1 );// - 128;
							a += a1>>1;
							b += b1>>1;
							
							a = (abs(a)*256*16)>>16 ;
							b = (abs(b)*256*16)>>16 ;


							if( a > 255 ) {
								a = 0;
							} else {
								a = costab[a];
							}

							if( b > 255 ) {
								b = 0;

							} else {
								b = costab[b];
							}

							a = a + b;

							 
							if( a > 80000 ) {
								hole = true;
							}

						}
						
					}

					u8 targetv = *target;
					
					if( hole ) {
						//*target++ = CELL_AIR;
						
					} else {

						//elevation += mountainmap[(16+cx)+(16+cz)*48];

						//elevation += sample_2d_cosine_ft((x*16+cx)*4,(z*16+cz)*4,0xFFffffFF,0) >> 4;
						if( ty <= elevation ) {
							if( ty > elevation - 8 +(mountain_elevation>>2)) { // 
								*target = CELL_DIRT;
							} else {
								//int a = sample_rnd( (tx)*548 + tz*1388 + ty *5182, 0, 0 );
								//int index =0;// mountainshades[ cx+( cz)*16];//mountains >> 5;
								int index =0;// mountainshades[(20+(cx+turba ))+(20+(cz+turbb ))*56];
								//index -= 5;
								//if( index < 0) index = 0;
								//if( index > 3 ) index = 3;

								if( ty > original_elevation ) {
									heightsource_x = (10*256) + (cx*64  + ((turbx*640)>>15)); // turb * 30/4 * 256 / (16bit)
									heightsource_y = (10*256) + (cz*64  + ((turbz*640)>>15));
									u8 *mountainshades_pointer = &mountainshades[(heightsource_x>>8)+(heightsource_y>>8)*24];
									index = lerp2(
										mountainshades_pointer[0],
										mountainshades_pointer[1],
										mountainshades_pointer[24],
										mountainshades_pointer[25],
										heightsource_x & 0xFF,
										heightsource_y & 0xFF );
									index = (index +128)>> 8;
									if( index < 0) index = 0;
									if( index > 3 ) index = 3;
								} else {

								}

								*target = CELL_MOUNTAIN1 + index;
								//*target = CELL_ROCK;//a > 124 ? CELL_GEMSTONE : CELL_ROCK;
							}
							
						//else if( ty == elevation ) {
						//	*target = mountains < 64 ? CELL_DIRT : CELL_ROCK;
						//	if( ty < SEA_LEVEL )
						//		*target = CELL_DIRT;
							
						} else if( ty <= SEA_LEVEL ) {
							*target = CELL_WATER;
						//} else {
						//	*target = CELL_AIR;
						}
					}
				} else {

					if( ty <= SEA_LEVEL )
						*target = CELL_WATER;
						
					//else
					//	ch->data[cx+cy*256+cz*16] = CELL_AIR;
				}

				target++;
				tx++;
			} // x
			tz++;
		} // z
		ty++;
	} // y

	

	u8 treemap[48*48];

	// top painter
	for( int cx = 0; cx < 16; cx++ ) {
		for( int cz = 0; cz < 16; cz++ ) {

			// unturbuled elevation
			int heightsource_x = (3*256) + (cx*16 ); // turb * 30 / 16 * 256 / (16bit)
			int heightsource_y = (3*256) + (cz*16 );
			u8 *heightsource_pointer = heightsource + (heightsource_x>>8) + (heightsource_y>>8)*8;
			int elevation = lerp2( 
				heightsource_pointer[0],
				heightsource_pointer[1],
				heightsource_pointer[8], 
				heightsource_pointer[9], heightsource_x & 0xFF, heightsource_y & 0xFF );

			heightsource_x = (10*256) + (cx*64 ); // turb * 30/4 * 256 / (16bit)
			heightsource_y = (10*256) + (cz*64 );
			short *mountainsource_pointer = &tmountainmap[(heightsource_x>>8)+(heightsource_y>>8)*24];
			int mountain_elevation = lerp2(
				mountainsource_pointer[0],
				mountainsource_pointer[1],
				mountainsource_pointer[24],
				mountainsource_pointer[25],
				heightsource_x & 0xFF,
				heightsource_y & 0xFF );
				
			elevation += mountain_elevation;
			
			elevation = (elevation + 128) >> 8;
			elevation = elevation - 128 + SEA_LEVEL;

			int start,end;
			start = elevation+5;
			end = start-10;
			
			
			for( int ty = start; ty >= end; ty-- ) {
				int cy = ty - (y*16);
				if( cy < 0 ) continue;
				if( cy > 15 )  continue;
				if( output[cx+(cy+1)*256+cz*16] == CELL_DIRT ) {
					if( output[cx+(cy+2)*256+cz*16] == CELL_AIR ) {
						output[cx+(cy+1)*256+cz*16] = CELL_GRASS;
						break;
					}
				}
				
				if( ch->data[cx+(cy+1)*256+cz*16] == CELL_WATER ) {
					break;
				}
			}
		}
	}

#endif

	const map_param_h *pbase = j->ct->map.get_data_h() + (x*256 + z*j->ct->map.width*256);
	/*
	for( int i = 0; i < 16*16*16; i++ ) {
		int cx = (i&15);
		int cy = (i>>8);
		int cz = ((i>>4)&15);
		if( y*16+cy < pbase[cx+cz*16].params[MPH_HEIGHT]+500 ) {
			output[i+256] = CELL_GRASS;
		} else {
			output[i+256] = CELL_AIR;
		}
		
	}
	*/
	// COPY RESULT
	for( int i = 0; i < 16*16*16; i++ ) {
		ch->data[i] = output[i+256];
	}
	
#if 0 // bypass tree plotter
	for( int cx = 12; cx < 36; cx++ ) {
		for( int cz = 12; cz < 36; cz++ ) {

			int elevation = theightmap[( (cx))+( (cz))*48];
			elevation = elevation - 128 + SEA_LEVEL;

			int tx = x*16-16+cx;
			int ty = elevation;
			int tz = z*16-16+cz;

			if( ty < SEA_LEVEL ) continue;
			int cy = ty - y*16;
			if( cy < -32 ) continue; // height too low

			int f = forestation[(cx>>16)+(cz>>16)*4];
			f=50;
			if( f != 0 ) {
			


				int density_threshold = elevation - ty;
				density_threshold -= 32;

				if( density_threshold > 0 ) density_threshold = 0;
				density_threshold = -density_threshold;

				int a = DENSITY_TEST;
				a = (a * density_threshold) >> 5;
			
				if( !(a > density) ) {

					// random number * high frequency noise * low frequency noise
				

					int a = sample_rnd( (tx)*548, (tz)*1388, 0 );
					int a2 = sample_rnd( (tx)*208, (tz)*1388, 525 );
					
					int tree;

					int b;
					b = sample_2d_cosine_ft_n((tx) * 4, (tz) * 4, 0) * 4;
					b += sample_2d_cosine_ft_n((tx) * 8, (tz) * 8, 0) * 2;
					b += sample_2d_cosine_ft_n((tx) * 16, (tz) * 16, 0) * 1;
					b = (b * 37) >> 8;
					b = b * 2;
					if( b>256) b=256;
					 
					int c;
					c = sample_2d_cosine_ft_n((tx) * 4, (tz) * 4, 0) * 4;
					c += sample_2d_cosine_ft_n((tx) * 8, (tz) * 8, 543) * 2;
					c += sample_2d_cosine_ft_n((tx) * 16, (tz) * 16, 6262) * 1;
					c = (c * 37) >> 8;
					c = ((abs(128 - c))) * 2;
					c = 256 - c;
					c = (c * c) >> 8;
					c = (c * c) >> 8;
					c = (c * c) >> 8;

					c = 256 - c;
					c = c + 64;

					int f2;

					f2 = ((f*b)>>8) / 2;
					f2 = (f2*c)>>8;
 
					if( a < f2 && a2 < f2 ) {
					
						
						plant_tree( ch, cx-16, cy, cz-16, x+z*541+a+a2);
						//plant_tree(ch,cx-16,cy,cz-16);

						//if( cy >= 0 && cy < 16 ) {
						//	if( cx >= 16 && cx <= 31 && cz >= 16 && cz <= 31 ) {
						//		ch->data[(cx-16)+(cz-16)*16+cy*256] = CELL_ROCK;
						//	}
						//}
					} else {

						if(cx >= 16 && cx <= 31 && cz >= 16 && cz <= 31) {
							if( a < f *2 && a < b/2 ) {
								if( cy >= -1 && cy < 15 ) {
									if( ch->data[cx-16+(cy+1)*256+(cz-16)*16] == CELL_AIR ) {
										ch->data[cx-16+(cy+1)*256+(cz-16)*16] = CELL_TALLGRASS;
									}
								}
							}
						}
					}
				} // } density check

			} // if (f != 0)
		}
	}
#endif
	 
	////// GENERATION CODE END //////

	test_solid_faces( ch );
	bool empty=1;
	for(int i = 0; i < 16*16*16; i++ ) {
		if( ch->data[i] != CELL_AIR ) {
			empty=0;
			break;
		}
	}

	if( empty ) {
		ch->table_index = -1;
		// TODO: MUST STILL DIRTY SIDES	
		InterlockedExchange( &j->ct->chunks.table[j->index], CHUNK_EMPTY );
		return;
	}

	/*
	ch->all_water=1;
	for( int i=0; i < 16*16*16; i++ ) {
		if( ch->data[i] != CELL_WATER ) {
			ch->all_water=0;
			break;
		}
	}*/

	ch->ct = j->ct;
	ch->table_index = j->index;
	//ch->instance = 0;
	ch->busy = 0;
	ch->touched = 0;
	//ch->dirty = 0;
	ch->modified = 0;
	//ch->sun_init = 1;
	//ch->dirty_sides = 1;

	InterlockedExchange( &j->ct->chunks.table[ch->table_index], (ch->memory_index<<8) | CHUNK_READY );

	//ct_current->register_height_sunlight(x,y,z);
	add_dirty_job(x,y,z,true,true,true);
	//dirty_sides(x,y,z,true,true,true);
	
	//dirty_sides(x,y,z); // todo: thread safety
	//register_chunk_height( x,y,z ); // todo: thread safety
	//register_sunlight_modification(x,y,z); // todo: thread safety

	
}

}

#ifndef AB_SERVERSIDE
/*
chunk *create_network_chunk( int x, int y, int z ) {
	chunk *ch = new_chunk();//new chunk;
	memset( ch, 0, sizeof( chunk ) );
	ch->network_request = NETWORK_REQUEST_NEEDED;
	ch->solid = CHUNK_SOLID_ALL;
	world->data[ x | (y*PLANET_WIDTH16*PLANET_WIDTH16) | (z*PLANET_WIDTH16) ] = ch;
	return ch;
}
*/
#endif

//bool world_exists() {
//	return world != NULL;
//}
//bool world_loaded() {
//	return b_world_loaded;
//}


//const u8 light_map[] = {15,15,15,15, 15,15,15,15, 15,15,15,15, 0,1,2,3};

//int get_opacity( chunk *ch, int cx, int cy, int cz ) {
//	return light_map[ch->data[cx+cy*256+cz*16]&15];
//}

//chunk *get_chunk_direct_indexed( int index ) {
//	int x = index & (PLANET_WIDTH16-1);
//	int z = (index>>PLANET_WIDTH_BITS) & (PLANET_WIDTH16-1);
//	int y = (index>>(PLANET_WIDTH_BITS*2));
//	return get_chunk_direct(x,y,z);
//}

CHUNKPTR *get_chunk_slot( int x, int y, int z ) {
	return ct_current->chunks.get_pointer( x,y,z );
}

CHUNKPTR get_chunk_pointer( int x, int y, int z ) {
	return ct_current->chunks.get( x, y, z );

	//if( x < 0 || z < 0 || x >= PLANET_WIDTH16 || z >= PLANET_WIDTH16 || y < 0 || y >= PLANET_HEIGHT16 ) return CHUNK_EMPTY;
	//CHUNK_STATE c = planet_data[ x | (y*PLANET_WIDTH16*PLANET_WIDTH16) | (z*PLANET_WIDTH16) ];
	//return c;
}

float get_sea_level() {
	return (float)ct_current->sea_level;
}
/*
CHUNKPTR get_chunk_pointer_wrapped( int x, int y, int z ) {
	if( y < 0 || y >= ct_current->chunks.height16 ) return CHUNK_EMPTY;
	x &= ct_current->chunks.width16-1;
	z &= ct_current->chunks.length16-1;
	return ct_current->chunks.get(x,y,z);
}*/

CHUNKPTR get_chunk_pointer( int world_index ) {
	return ct_current->chunks.get(world_index);
}

chunk *get_chunk( CHUNKPTR c ) {
	return real_chunk(c) ? to_chunk(c) : 0;
} 

void refresh_chunk( CHUNKPTR p ) {
	chunk *ch = get_chunk(p);
	if( ch ) chunk_manager.bump( ch );
}
/*
bool is_sunlight_dirty( int x, int z ) {
	if( !ct_current ) return false;
	return ct_current->is_sunlight_dirty(x,z);
}*/

int get_depth() {
	return ct_current->depth;
}

int get_width() {
	return ct_current->width;
}

int get_length() {
	return ct_current->length;
}

bool collision( int x, int y, int z ) {
	bool d;
	CHUNKPTR cx = get_chunk_pointer( x>>4, y>>4, z>>4 );
	
	
	if( read_state(cx) == CHUNK_EMPTY ) {
		d = false;
	} else if( !real_chunk(cx) ) {
		d = true;
	} else {
		chunk *c = to_chunk(cx) ;
		d = !!collision_map[c->data[ (x&15) | ((y&15)<<8) | ((z&15)<<4) ]];
	}
	return d;
}

bool collision3f( cml::vector3f pos ) {
	return collision( (int)floor(pos[0]), (int)floor(pos[1]), (int)floor(pos[2]) );
}
bool collision3d( cml::vector3d pos ) {
	return collision( (int)floor(pos[0]), (int)floor(pos[1]), (int)floor(pos[2]) );
}

int get_cube( int x, int y, int z ) {
	CHUNKPTR p = get_chunk_pointer( x>>4, y>>4, z>>4 );
	
	if( real_chunk(p) ) {
		chunk *c = to_chunk(p);
		return c->data[(x&15)+(y&15)*256+(z&15)*16];
	} else {
		return read_state(p) == CHUNK_EMPTY ? CELL_AIR : CELL_ROCK;
	}
}

void set_context( context *ct ) {
	ct_current = ct;
}

void process_modification_dr_chunk( modification *m, int cx, int cy, int cz, chunk *ch ) {
	 

	int chunkx, chunky, chunkz;
	chunkx = (m->param[0] & ~15) + cx*16;
	chunky = (m->param[1] & ~15) + cy*16;
	chunkz = (m->param[2] & ~15) + cz*16;

	int sx,ex, sy, ey, sz, ez; // start,end points
	sx = m->param[0] - m->param[3]; ex = m->param[0] + m->param[3];
	sy = m->param[1] - m->param[3]; ey = m->param[1] + m->param[3];
	sz = m->param[2] - m->param[3]; ez = m->param[2] + m->param[3];

	if( sx < chunkx ) sx = chunkx;
	if( sx > chunkx + 15 ) return;
	if( sy < chunky ) sy = chunky;
	if( sy > chunky + 15 ) return;
	if( sz < chunkz ) sz = chunkz;
	if( sz > chunkz + 15 ) return;

	if( ex < chunkx ) return;
	if( ex > chunkx+15 ) ex = chunkx+15;
	if( ey < chunky ) return;
	if( ey > chunky+15 ) ey = chunky+15;
	if( ez < chunkz ) return;
	if( ez > chunkz+15 ) ez = chunkz+15;

	int rr = m->param[3];
	rr *= rr;

	bool modified=false;
	
	for( int x = sx; x <= ex; x++ ) {
		for( int y = sy; y <= ey; y++ ) {
			for( int z = sz; z <= ez; z++ ) {

				// compute distance
				int dx = x - m->param[0];
				int dy = y - m->param[1];
				int dz = z - m->param[2];
				int d = dx*dx+dy*dy+dz*dz;

				if( d < rr ) {
					
					u8 *dat = ch->data + (x&15)+(y&15)*256+(z&15)*16;
					if( *dat != CELL_AIR ) {

						if( ((x&1)==(z&1))  ) {
							worldrender::terrain_particle( x,y,z, *dat );
							//particles::add( particles::PK_GRASS_PARTICLES, cml::vector3f(x,y,z) );
							//particles::add( particles::PK_DIRT_PARTICLES, cml::vector3f(x,y,z) );
						}
						*dat = CELL_AIR;
						modified=true;
					}
					

				}
			}
		}
	}
	if( modified ) {
		worldrender::dirty_area(chunkx>>4,chunky>>4,chunkz>>4,true,true,true);
//		dirty_sides(chunkx>>4,chunky>>4,chunkz>>4,true,true,true);
		worldrender::set_skylight_dirty( ct_current, chunkx>>4, chunky>>4, chunkz>>4 );
		//ct_current->register_sunlight(chunkx>>4,chunky>>4,chunkz>>4);
	}
}

void process_modification_dr( modification *m ) {
	if( m->param[4] == 0 ) {
		m->finished=true;
		return;
	}
	int cx, cy, cz;
	for( cy = 0; cy < 3; cy++ ) {
		for( cz = 0; cz < 3; cz++ ) {
			for( cx = 0; cx < 3; cx++ ) {
				int bitmask = (1<<(cx+cy*9+cz*3));
				if( !(m->param[4] & bitmask ) ) {
					continue;
				}

				int chunkx,chunky,chunkz;
				chunkx = (m->param[0]>>4)+(cx-1);
				chunky = (m->param[1]>>4)+(cy-1);
				chunkz = (m->param[2]>>4)+(cz-1);
				CHUNKPTR p = get_chunk_pointer( chunkx, chunky, chunkz );
				int state = read_state(p);
				if( !real_chunk(p) ) {
					if( state == CHUNK_INVALID ) {
						request(chunkx,chunky,chunkz);
						//continue;
					} else if( state == CHUNK_EMPTY ) {
						m->param[4] &= ~bitmask;
					} else {
						//continue;
						
					}
				} else {
					process_modification_dr_chunk( m, cx-1,cy-1,cz-1, get_chunk(p) );
					m->param[4] &= ~bitmask;
				}
			}
		}
	}

	if( m->param[4] == 0 ) m->finished = true;
}

void process_modification( modification *m ) {
	switch( m->type ) {
	case MOD_DESTROY_RADIUS:
		process_modification_dr(m);
	}
}

void destroy_radius( int x, int y, int z, int radius ) {
	modification *m = new modification;
	m->param[0] = x;
	m->param[1] = y;
	m->param[2] = z;
	m->param[3] = radius;
	m->param[4] = 0;
	m->type = MOD_DESTROY_RADIUS;
	m->finished = false;

	int ix,iy,iz;
	ix = x&15; iy = y&15; iz = z&15;

	//if( (ix - radius)<0 && (iy-radius)<0 && (iz-radius)<0 ) m->param[4] |= bitgrid_

	int cx,cy,cz;
	for( cx = -1; cx <= 1; cx++ ) {
		for( cy = -1; cy <= 1; cy++ ) {
			for( cz = -1; cz <= 1; cz++ ) {
				if( 
						((cx == 0) || ((ix+cx*radius) & 16))
					&&	((cy == 0) || ((iy+cy*radius) & 16))
					&&	((cz == 0) || ((iz+cz*radius) & 16)) ) {

					m->param[4] |= 1<<((cx+1)+(cy+1)*9+(cz+1)*3);
				}
			}
		}
	}
	
	process_modification(m);
	
	if( !m->finished ) {
		if( mods_last ) {
			mods_last->next = m;
			m->prev = mods_last;
			m->next = 0;
			mods_last = m;
		} else {
			mods_last = mods_first = m;
			m->prev = m->next = 0;
		}
	} else {
		delete m;
	}
}

void process_mods() {
	for( modification *m = mods_first; m; ) {
		modification *n = m->next;
		process_modification( m );

		if( m->finished ) {
			if( m->prev == 0 ) {
				mods_first = m->next;
			} else {
				m->prev->next = m->next;
			}

			if( m->next == 0 ) {
				mods_last = m->prev;
			} else {
				m->next->prev = m->prev;
			}
			delete m;
		}
		m = n;
	}
}

/*
void destroy_radius( int x, int y, int z, int radius ) {
	//particles::add( cml::vector3f(x,y,z), cml::vector3f(0,0,0), 0, 0, radius*1.5, radius*0.1, 2.0, -0.04, true, 48.0/512.0, 96.0/512.0,32.0/512.0);
	for( int cx = -radius; cx <= radius; cx++ ) {
		for( int cy = -radius; cy <= radius; cy++ ) {
			for( int cz = -radius; cz <= radius; cz++ ) {
				int c = cx*cx+cy*cy+cz*cz;
				if( c < radius*radius )  {
					int tcx,tcy,tcz;
					tcx=x+cx;
					tcy=y+cy;
					tcz=z+cz;
					chunk *ch = get_chunk_direct( tcx>>4,tcy>>4,tcz>>4 );
					if( ch == 0 )
						ch = generate_chunk( tcx>>4,tcy>>4,tcz>>4 );
					//else if( ch == CHUNK_EMPTY )
					//	ch = create_empty_chunk( tcx>>4,tcy>>4,tcz>>4 );
					if( valid_chunk_addr(ch) ) {

						//if( ch->data[ (tcx&15) + (tcy&15)*256 + (tcz&15)*16 ] != CELL_AIR ) {
							
								worldrender::terrain_particle( tcx,tcy,tcz, ch->data[ (tcx&15) + (tcy&15)*256 + (tcz&15)*16 ] );
							ch->data[ (tcx&15) + (tcy&15)*256 + (tcz&15)*16 ] = CELL_AIR;
							//ch->sunlight[ (tcx&15) + (tcy&15)*256 + (tcz&15)*16 ] = 15;
							//ch->sunlight[ (tcx&15) + (tcy&15)*256 + (tcz&15)*16 ] = 0;
							//ch->sunlight_invalid_bitmap |= (1<<(tcy&15));
							//ch->light_cache[ (tcx&15) + (tcy&15)*256 + (tcz&15)*16 ] = 0;
							ch->dirty=1;
							ch->modified=1;
							//ch->sunlight_changed=1;

							register_sunlight_modification(tcx>>4,tcy>>4,tcz>>4);
							
						//}
					}
				}
			}
		}
	}
}*/

/*
void set_cell( cml::vector3i pos, int cell ) {
	chunk *ch = get_chunk_direct( pos[0]>>4,pos[1]>>4,pos[2]>>4 );
	if( valid_chunk_addr(ch) ) {
		ch->data[(pos[0]&15)+(pos[1]&15)*256+(pos[2]&15)*16] = cell;
		ch->dirty=1;
		ch->modified=1;
		
		register_sunlight_modification( pos[0]>>4,pos[1]>>4,pos[2]>>4  );
	}
}*/
 
bool pick( const cml::vector3f &start, const cml::vector3f &end, cml::vector3f *intersection, cml::vector3i *collided, cml::vector3i *adjacent, cml::vector3f *normal  ) {
	cml::vector3f inc = end - start;

	cml::vector3f vec = start;
	inc.normalize();
	float length = cml::length(end-start);
	
	if( inc[0] == 0.0f && inc[1] == 0.0f && inc[2] == 0.0f ) return false;
	
	int ix, iy, iz;
	int icx, icy, icz;

	float dx,dy,dz;
	dx = inc[0] != 0.0f ? 1.0f / inc[0] : 99999;
	dy = inc[1] != 0.0f ? 1.0f / inc[1] : 99999;
	dz = inc[2] != 0.0f ? 1.0f / inc[2] : 99999;

	icx=icy=icz=-1;

	float total = 0.0f;

	chunk *ch;
	CHUNKPTR chi;

	for( ; total < length;  ) {

		cml::vector3f dist; // distances to next cell
		
		dist[0] = floor(vec[0]);
		dist[1] = floor(vec[1]);
		dist[2] = floor(vec[2]);
		if( inc[0] >= 0.0f ) dist[0] += 1.0f;
		if( inc[1] >= 0.0f ) dist[1] += 1.0f;
		if( inc[2] >= 0.0f ) dist[2] += 1.0f;

		dist -= vec;
		dist[0] *= dx;
		dist[1] *= dy;
		dist[2] *= dz;

		int dir=0; // left/right

		if( dist[1] < dist[0] ) {dist[0] = dist[1]; dir=1;} // top/bottom
		if( dist[2] < dist[0] ) {dist[0] = dist[2]; dir=2;} // front/back

		dist[0] += 0.01f;

		vec += inc*dist[0];
		if( total+dist[0] > length ) return false;
		total += dist[0];


		ix = (int)floor(vec[0]);
		iy = (int)floor(vec[1]);
		iz = (int)floor(vec[2]);

		if( (ix>>4) != icx || (iy>>4) != icy || (iz>>4) != icz ) {
			icx = ix>>4;
			icy = iy>>4;
			icz = iz>>4;

			chi = get_chunk_pointer( icx, icy, icz );
		}

		if( real_chunk(chi) ) {
			ch = to_chunk(chi);
			if( ch->data[(ix&15)+(iy&15)*256+(iz&15)*16] != CELL_AIR ) {
				///airpick(vec,-inc,intersection,collided,adjacent,normal);
				
				if( intersection ) {
					
 					*intersection = vec - inc*0.01;// - inc;
					
				}
				if( collided ) {
					(*collided)[0] = ix;
					(*collided)[1] = iy;
					(*collided)[2] = iz;
				}
				if( adjacent ) {
					(*adjacent)[0] = ix + ((dir==0) ? (inc[0] >= 0 ? -1 : 1) : 0);
					(*adjacent)[1] = iy + ((dir==1) ? (inc[1] >= 0 ? -1 : 1) : 0);
					(*adjacent)[2] = iz + ((dir==2) ? (inc[2] >= 0 ? -1 : 1) : 0);
				}
				if( normal ) {
					(*normal)[0] = (float)(((dir==0) ? (inc[0] >= 0 ? -1 : 1) : 0));
					(*normal)[1] = (float)(((dir==1) ? (inc[1] >= 0 ? -1 : 1) : 0));
					(*normal)[2] = (float)(((dir==2) ? (inc[2] >= 0 ? -1 : 1) : 0));
				}
				return true;
			}
		}
	}

	return false;
}

int random_number_index=0;
u8 random_number() {
	random_number_index = (random_number_index + 1) & SEED_MASK;
	return random_table[random_number_index].data[0];
}

//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
bool request( int world_index ) {
//-------------------------------------------------------------------------------------------------

	CHUNKPTR *ps = &ct_current->chunks.table[world_index];
	CHUNKPTR s = *ps;
	if( read_state(s) == CHUNK_BUSY ) {
		return false;
	}

	CHUNKPTR prev = InterlockedExchange( ps, CHUNK_BUSY );

	if( read_state(prev) == CHUNK_BUSY ) {
		return false;
	}

	job *j = new job;
	j->ch = chunk_manager.pop();
	j->ct = ct_current;
	j->index = world_index;
	j->type = JOB_REQUEST;

	job_fifo.push(j);

	ResumeThread( handleChunkLoader );

	return true;
}

//-------------------------------------------------------------------------------------------------
bool request( int x, int y, int z ) {
//-------------------------------------------------------------------------------------------------
	return request( ct_current->chunks.index_from_coords(x,y,z) );
}


/*
void add_job( int type, int p1, int p2, int p3, int p4, int p5, int p6 ) {
	job j;
	j.type=type;
	j.p1=p1;
	j.p2=p2;
	j.p3=p3;
	j.p4=p4;
	j.p5=p5;
	j.p6=p6;
	jobs.push( j );
}

void queue_generation( int x, int y, int z ) {
	planet_data[ x + y * PLANET_WIDTH16 * PLANET_WIDTH16 + z * PLANET_WIDTH16 ] = CHUNK_JOB;
	add_job( JOB_GENERATE, x, y, z, 0, 0, 0 );
}
*/
//void queue_lighting( int x1, int y1, int z1, int x2, int y2, int z2 ) {
//	add_job( JOB_LIGHTING, x1, y1, z1, x2, y2, z2 );
//}
/*
void do_job( job &a ) {
	switch( a.type ) {
	case JOB_GENERATE:
		generate_chunk( a.p1, a.p2, a.p3 );
		break;
	//case JOB_SUNLIGHT:
		//recalc_sunlight( a.p1, a.p2, a.p3, a.p4 );
	//	break;
	}
}
*/
//void do_jobs() {
//	while( !jobs.empty() ) {
//		do_job( jobs.front() );
//		jobs.pop();
//	}
//}

//-------------------------------------------------------------------------------------------------
DWORD WINAPI ChunkLoadingRoutine( LPVOID lpParam ) {
//-------------------------------------------------------------------------------------------------
	while(threadChunkLoaderRunning) {
		
		job *j = job_fifo.pop();
		if(j) {
			switch(j->type) {
			case JOB_REQUEST:
				WORKTHREAD::generate_chunk(j);
				delete j;
			}
		} else {

			SwitchToThread();
		}
	}
	return 0;
}

//-------------------------------------------------------------------------------------------------
void initialize() { 
//-------------------------------------------------------------------------------------------------

	setup_random_table(8484);
	
	allocate_chunk_memory( 40000 );
	threadChunkLoaderRunning=true;
	handleChunkLoader = CreateThread( NULL, 0, ChunkLoadingRoutine, 0, CREATE_SUSPENDED, &threadChunkLoader );

	mods_first=mods_last=0;

	initialize_tree_stamps();
}

//-------------------------------------------------------------------------------------------------
void unload() {
//-------------------------------------------------------------------------------------------------
	delete_tree_stamps();
}


//-------------------------------------------------------------------------------------------------
world_parameters::world_parameters() {
//-------------------------------------------------------------------------------------------------
	loaded = false;
	data=0;
	data_h=0;
}

//-------------------------------------------------------------------------------------------------
world_parameters::~world_parameters() {
//-------------------------------------------------------------------------------------------------
	clean();
}

//-------------------------------------------------------------------------------------------------
void world_parameters::clean() {
//-------------------------------------------------------------------------------------------------
	if(data) delete[] data;
	if(data_h) delete[] data_h;
	data=0;
	data_h=0;
	loaded=false;
}

//-------------------------------------------------------------------------------------------------
map_param * world_parameters::get_data() {
//-------------------------------------------------------------------------------------------------
	return data;
}

//-------------------------------------------------------------------------------------------------
map_param_h * world_parameters::get_data_h() {
//-------------------------------------------------------------------------------------------------
	return data_h;
}

//-------------------------------------------------------------------------------------------------
map_param * world_parameters::read_data( int x, int z ) {
//-------------------------------------------------------------------------------------------------
	if( x < 0 || z < 0 || x >= width || z >= length ) return 0;
	return &data[x+z*width];
}

//-------------------------------------------------------------------------------------------------
map_param_h * world_parameters::read_data_h( int xi, int zi, int x, int z ) {
//-------------------------------------------------------------------------------------------------
	if( xi < 0 || zi < 0 || xi >= width || zi >= length ) return 0;
	return &data_h[ (xi+zi*width)*256 + x + z*16 ];
}

//-------------------------------------------------------------------------------------------------
void world_parameters::load( const char *file ) {
//-------------------------------------------------------------------------------------------------
	if( loaded ) {
		clean();
	}
	filepath=file;

	BinaryFile f;
	f.open(file,BinaryFile::MODE_READ);
	f.read32(); // 'abws'
	f.read32(); // version

	f.readstring( worldname, 64 );
	width   = f.read16();
	length  = f.read16();
	depth   = f.read16();
	kernel  = f.read16();
	int compression = f.read16();
	sea_level = f.read16();
	f.seek(256);
	
	data = new map_param[ width*length ];
	data_h = new map_param_h[ width*length*256 ];

	for( int i = 0; i < width*length*256; i++ ) data_h[i].params[MPH_HEIGHT] = f.read8();
	for( int i = 0; i < width*length*256; i++ ) data_h[i].params[MPH_OVERLAY] = f.read8();

	for( int param = 0; param < 3; param++ ) {
		for( int i = 0; i < width*length; i++ )  {
			data[i].params[param] = f.read8();
		}
	}

	if( !load_zoneforms_cache() ) {
		upscale_zone_forms();
		save_zoneforms_cache();
	}

	////////////////////////////////////////////////////////////////// upscaled zoneform dump test
	/*
	u8 *test_data;;
	test_data = new u8[3*width*16*length*16];
	for( int x = 0; x < width*16; x++ ) {
		for( int y = 0; y < length*16; y++ ) {
			int index = (x%16)+(y%16)*16+(x/16)*256+(y/16)*width*256;
			//int index = (x/16) +(y/16)*width ;

			int color = data_h[index].params[MPH_ZONES];
			test_data[(x+y*width*16)*3] = color*31;
			test_data[(x+y*width*16)*3+1] = color*55;
			test_data[(x+y*width*16)*3+2] = color*99;
		}

	}

	debug_dump( width*16,length*16,test_data );
	delete[] test_data;*/
	///////////////////////////////////////////////////////////////////

	if( !load_sealevel_cache() ) {
		compute_sealevel();
		save_sealevel_cache();
	}
	
	
	/*
	for( int i = 0; i < width*length; i++ ){
		f.readbytes( data[i].params, 8 );              
	}*/

	f.close();

	loaded=true;

}

//-------------------------------------------------------------------------------------------------
std::string world_parameters::zoneform_cache_path() {
//-------------------------------------------------------------------------------------------------
	
	std::string path = filepath;
	path.append( ".zonecache" );
	return path;
}

//-------------------------------------------------------------------------------------------------
std::string world_parameters::sealevel_cache_path() {
//-------------------------------------------------------------------------------------------------
	std::string path = filepath;
	path.append( ".seacache" );
	return path;
}

//-------------------------------------------------------------------------------------------------
bool world_parameters::load_zoneforms_cache() {
//-------------------------------------------------------------------------------------------------
	BinaryFile f;

	f.open( zoneform_cache_path().c_str(), BinaryFile::MODE_READ );
	if( !f.isopen() ) {
		return false;
	}

	for( int i = 0; i < width*length*256; i++ ) data_h[i].params[MPH_ZONES] = f.read8();

	f.close();

	return true;
}

//-------------------------------------------------------------------------------------------------
void world_parameters::save_zoneforms_cache() {
//-------------------------------------------------------------------------------------------------
	BinaryFile f;

	f.open( zoneform_cache_path().c_str(), BinaryFile::MODE_WRITE );
	
	for( int i = 0; i < width*length*256; i++ ) f.write8( data_h[i].params[MPH_ZONES] );

	f.close();
}

//-------------------------------------------------------------------------------------------------
bool world_parameters::load_sealevel_cache() {
//-------------------------------------------------------------------------------------------------
	BinaryFile f;
	f.open( sealevel_cache_path().c_str(), BinaryFile::MODE_READ );
	if( !f.isopen() ) {
		return false;
	}

	for( int i = 0; i < width*length*256; i++ ) data_h[i].params[MPH_SEALEVEL] = f.read8();

	f.close();

	return true;
}

//-------------------------------------------------------------------------------------------------
void world_parameters::save_sealevel_cache() {
//-------------------------------------------------------------------------------------------------
	BinaryFile f;
	f.open( sealevel_cache_path().c_str(), BinaryFile::MODE_WRITE );

	for( int i = 0; i < width*length*256; i++ ) f.write8( data_h[i].params[MPH_SEALEVEL] );

	f.close();
}

#define seastamp_size 512


//-------------------------------------------------------------------------------------------------
void world_parameters::paint_stamp( int x, int z, u8 *stamp, u8 *temp ) {
//-------------------------------------------------------------------------------------------------
	int px,pz;
	for( pz = 0; pz < seastamp_size+1; pz++ ) {
		for( px = 0; px < seastamp_size+1; px++ ) {
			int spx = px-seastamp_size/2;
			int spz = pz-seastamp_size/2;
			if( x+spx < 0 || x+spx >= width*16 ) continue;
			if( z+spz < 0 || z+spz >= length*16 ) continue;
			spx += x;
			spz += z;
			int stamp_sample = stamp[px+pz*(seastamp_size+1)];

			if( temp[spx+spz*width*16] >= 128 ) { // land
				if( temp[spx+spz*width*16] > 128+stamp_sample ) 
					temp[spx+spz*width*16] = 128+stamp_sample;
			} else {
				if( temp[spx+spz*width*16] < 127-stamp_sample )
					temp[spx+spz*width*16] = 127-stamp_sample;
			}
		}
	}
}

//-------------------------------------------------------------------------------------------------
void world_parameters::compute_sealevel() {
//-------------------------------------------------------------------------------------------------
	

	// create circular stamp

	// copy and convert zone forms into binary map (land/sea)
	u8 *temp;
	temp = new u8[width*16*length*16];
	for( int i = 0; i < width*16*length*16; i++ ) {
		// swizzle data and apply threshold function
		int source, dest;
		int sx = i & ((width*16)-1);
		int sz = i / (width*16);
		source = (sx&15)+(sx/16)*256+(sz&15)*16+(sz/16)*width*256;
		dest = i;
		int a = data_h[source].params[MPH_ZONES] == C1_ZONE_SEA ? 0 : 255;
		temp[i] = a;
	}

	u8  stamp[(seastamp_size+1)*(seastamp_size+1)];

	for( int x = 0; x < seastamp_size+1; x++ ) {
		for( int y = 0; y < seastamp_size+1; y++ ) {
			int dx = seastamp_size/2-x, dy = seastamp_size/2-y;
			int d = dx*dx+dy*dy;
			d = (int)(sqrt((double)d) * (127.0 / (seastamp_size/2.0)));
			if( d > 127 ) d = 127;
			stamp[x+y*(seastamp_size+1)] = d;
		}
	}
	
	for( int z = 1; z < length*16-1; z++ ) {
		for( int x = 1; x < width*16-1; x++ ) {
			if( temp[x+z*width*16] >= 128 ) {
				if( temp[x-1 + z*width*16] < 128 ||
					temp[x+1 + z*width*16] < 128 ||
					temp[x + (z-1)*width*16] < 128 ||
					temp[x + (z+1)*width*16] < 128 )  {

					// paint here
					paint_stamp( x, z, stamp, temp );
				}
			}
		}
	}

	// copy/swizzle to param data
	for( int i = 0; i < width*16*length*16; i++ ) {
		
		int source, dest;
		int sx = i & ((width*16)-1);
		int sz = i / (width*16);
		source = i; 
		dest = (sx&15)+(sx/16)*256+(sz&15)*16+(sz/16)*width*256;
		int a = temp[source];
		data_h[dest].params[MPH_SEALEVEL] = a;
	}

	////////////////////////////////////////////////////
	
	u8 *test_data;
	test_data = new u8[width*16*length*16*3];

	for( int x = 0; x < width*16; x++ ) {
		for( int z = 0; z < length*16; z++ ) {
			int index = (x%16)+(z%16)*16+(x/16)*256+(z/16)*width*256;
			int color = data_h[index].params[MPH_SEALEVEL];
			test_data[(x+z*width*16)*3] = color;
			test_data[(x+z*width*16)*3+1] = color;
			test_data[(x+z*width*16)*3+2] = color;
		}
	}
	debug_dump( width*16, length*16, test_data );
	delete[] test_data;
	
	delete[] temp;
}

//-------------------------------------------------------------------------------------------------
void world_parameters::upscale_zone_forms() {
//-------------------------------------------------------------------------------------------------
	// this is a function that passes over the zone data
	// the upscaled data (16x) is formed by doing a turbulated sampling filter under a box filter to
	// determine which zone is dominant for that single upscaled cell

	

	for( int cz = 0; cz < length; cz++ )  {
		for( int cx = 0; cx < width; cx++ ) {
			for( int z = 0; z < 16; z++ ) {
				for( int x = 0; x < 16; x++ ) {
					int tx = cx*16+x;
					int tz = cz*16+z;


					short sample_counts[256];
					for( int i = 0; i < 256; i++ ) {
						sample_counts[i] = 0;

					}

					int gaussian_range = 8;
					
					// BOX FILTER
					int gx,gz;
					for( gz = -gaussian_range; gz <= gaussian_range; gz++) {
						for( gx = -gaussian_range; gx <= gaussian_range; gx++ ) {
							int turbulence_x = sample_bilinear( (tx+gx)*16, (tz+gz)*16, 0 ) * 16/65536-8;
							int turbulence_z = sample_bilinear( (tx+gx)*16, (tz+gz)*16, 4444 ) * 16/65536-8;
							int sample_x = tx + turbulence_x + gx;
							int sample_z = tz + turbulence_z + gz;
							sample_x = sample_x < 0 ? 0 : sample_x;
							sample_x = sample_x >= width*16  ? width*16-1  : sample_x;
							sample_z = sample_z < 0 ? 0 : sample_z;
							sample_z = sample_z >= length*16 ? length*16-1 : sample_z;
							int sample = data[(sample_x>>4)+(sample_z>>4)*width].params[MP_ZONE];
							sample_counts[sample]++;
						}
					}

					int max_index = 16;

					int best_count=0, best_index=0;
					for( int i = 0; i < max_index; i++ ) {
						if( sample_counts[i] > sample_counts[best_index] ) {
							best_index = i;
							best_count = sample_counts[i];
						}
					}

					data_h[ cx*256 + x + z*16 + cz*256*width ].params[MPH_ZONES] = best_index;
				}
			}
		}
	}
	 
	
}


};

#endif
