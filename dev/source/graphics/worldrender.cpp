//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//


#include "stdafx.h"
#if 0 // bypass


int debug_instance_count=0;
int debug_count_2=0;

typedef struct t_plane32 {
	cml::vector3f normal;
	float d;
} plane32;

typedef struct t_plane2 {
	cml::vector3f normal;
	cml::vector3d point;
} plane2;

namespace worldrender {

void dirty_pancake( int x, int y, int z, bool geometry, bool dimlight, bool skylight );
void dirty_area( int x, int y, int z, bool geometry, bool dimlight, bool skylight );

#define instanceCount 1000


class renderingList;

typedef struct t_skylight_data {
	int references;
	int index;
	s16 dirty[4]; // -1 = not dirty, 0x7FFF = FULL DIRTY! MEASURED IN CHUNIKS

	u16 depth[16*16*4];
	u16 lowest_depth[4];

	struct t_skylight_data *next;
	struct t_skylight_data *prev;
} skylight_data;

class SkylightManager {
	LinkedList<skylight_data> free_instances;
	LinkedList<skylight_data> used_instances;
	
	u16 *data_map;

	int data_map_width;

	skylight_data *sm_instances;

	planet::context *ct;

	skylight_data *NewInstance() {
		skylight_data *a = free_instances.GetFirst();
		free_instances.Remove( a );
		used_instances.Add( a );
		return a;
	}

	void DeleteInstance( skylight_data *i ) {
		used_instances.Remove(i);
		free_instances.Add(i);

	}

	skylight_data * GetInstance( int x, int z ) {
		int index = data_map[x+z*data_map_width];
		if( index > 0 ) {
			return sm_instances+index-1;

		}
		return 0;
	}

	skylight_data * GetInstanceEC( int x, int z ) {
		// ec = extra checking
		if( x < 0 || z < 0 || x >= data_map_width || z >= data_map_width ) return 0;
		int index = data_map[x+z*data_map_width];
		if( index > 0 ) {
			return sm_instances+index-1;

		}
		return 0;
	}

	void UpdateDepthSingle( skylight_data *instance, int ix, int iz, int offset ) {
		//x,z = planet coordinates
		//offset = 0-3 (+0,+x,+z,+xz)
		int lowest_point = ct->depth16;
		int level = instance->dirty[offset];
		planet::chunk *ch;

		int x = ix*2+(offset&1);
		int z = iz*2+(offset>>1);

		u16 *depth = instance->depth + offset*16*16;
		
		for( int cx = 0; cx < 16; cx++ ) {
			for( int cz = 0; cz < 16; cz++ ) {
				
				int ty = level*16+15;
				


				if( ty < depth[cx+cz*16] ) {
					ty = depth[cx+cz*16];
				//	if( (ty>>4) < lowest_point ) lowest_point = (ty>>4);
				//	continue;
				}

				ty = ty > (ct->depth-1) ? (ct->depth-1) : ty;
				
				
				
				bool found = false;
				while( ty > 0 ) {
					
					ch = planet::get_chunk( planet::get_chunk_pointer( x, ty>>4, z ) );
					if( ch ) {
						// pooda

						for( int i = 0; i < 16; i++ ) {
							if( ch->data[cx+(ty&15)*256+cz*16] != planet::CELL_AIR ) {
								// break ray
								ty++;
								found=true;
								break;
								
							}
								
							ty--;
						}

						if( found ) break;
					} else {
						if( ty <= (ct->sea_level) ) {
							ty++;
							// assume solid past sea level
							break;
						}
						ty -= 16;
					}
				}
				if( ty < 0 ) ty = 0;

				depth[cx+cz*16] = ty;
				if( (ty>>4) < lowest_point ) lowest_point = (ty>>4);
				
			}

		}

		int lower_dirty_point = instance->lowest_depth[offset] < lowest_point ? instance->lowest_depth[offset] : lowest_point;

		// todo: modified range is dirty -> lowest point
		for( int cy = instance->dirty[offset]; cy >= lower_dirty_point; cy-- ) {
		
			dirty_pancake( x, cy, z, true, false, true );
		}
		dirty_pancake( x, (instance->dirty[offset])+1, z, true, false, true );
		dirty_pancake( x, lower_dirty_point-1, z, true, false, true );
		instance->lowest_depth[offset] = lowest_point;
		instance->dirty[offset] = -1;
	}

public:
	SkylightManager() {
		sm_instances = 0;
	}

	~SkylightManager() {
		if( sm_instances ) delete[] sm_instances;
		sm_instances = 0;
	}

	void Init() {
		data_map_width = 512;
		sm_instances = new skylight_data[instanceCount];
		data_map = new u16[data_map_width*data_map_width];
		assert( data_map );
		Reset();
		
	}

	void Reset() {
		free_instances.Erase();
		used_instances.Erase();
		for( int i = 0; i < instanceCount; i++ ) {
			sm_instances[i].references = 0;
			sm_instances[i].index = i;
			for( int j = 0; j < 4; j++ ) {
				sm_instances[i].dirty[j] = -1;
				sm_instances[i].lowest_depth[j] = 0;
			}
			free_instances.Add( sm_instances + i );
		}
		for( int i = 0; i < data_map_width*data_map_width; i++ ) {
			data_map[i] = 0;
		}
	}

	void ChangeContext( planet::context *p_context ) {
		ct = p_context;
		Reset();
	}
	
	skylight_data *Register( int x, int z ) {
		assert( ct );
		assert( x >= 0 && z >= 0 && x < data_map_width && z < data_map_width );
		int index = data_map[x+z*data_map_width];

		skylight_data *n;

		if( index > 0 ) {
			n = sm_instances + index-1;
			
		} else {
			n = NewInstance();
			
			data_map[x+z*data_map_width] = n->index+1;
			for( int i = 0; i < 4; i++ ) {
				n->dirty[i] = 0x7FFF;
				n->lowest_depth[i] = 0;
			}

			for( int i = 0; i < 16*16*4; i++ ) {
				n->depth[i] = 0x0000;
			}
		}

		n->references++;
		
		return n;
	}

	void Unregister( int x, int z ) {
		assert( ct );
		assert( x >= 0 && z >= 0 && x < data_map_width && z < data_map_width );
		int index = data_map[x+z*data_map_width];
		
		assert(index > 0); // index must be > 0

		sm_instances[index-1].references--;
		if( sm_instances[index-1].references == 0 ) {
			data_map[x+z*data_map_width] = 0;
			DeleteInstance( &sm_instances[index-1] );

		}
	}

	
	

	void SetDirty( int x, int y, int z ) {
		skylight_data *i = GetInstance(x>>1,z>>1);
		
		if( i ) {
			//y = y * 16 + 15;
			int index = (x&1) + (z&1)*2;
			if( y > i->dirty[index] ) i->dirty[index] = y;
		}
	}

	void Update( int x, int z ) {
		skylight_data *i = GetInstance(x,z);
		if( i ) {
			
			if( i->dirty[0] >= i->lowest_depth[0]-1 ) UpdateDepthSingle(i,x,z,0);
			if( i->dirty[1] >= i->lowest_depth[1]-1 ) UpdateDepthSingle(i,x,z,1);
			if( i->dirty[2] >= i->lowest_depth[2]-1 ) UpdateDepthSingle(i,x,z,2);
			if( i->dirty[3] >= i->lowest_depth[3]-1 ) UpdateDepthSingle(i,x,z,3);
		 
		}
	}

	bool IsDirty( int x, int z ) {
		// xz = planet coordinates
		skylight_data *data = GetInstanceEC( x>>1, z>>1 );
		if( data > 0 ) {
			int index = (x&1) + (z&1)*2;
			return data->dirty[index] >= data->lowest_depth[index]-1;
		}
		return false;
	}

	u16 *GetData( int x, int z ) {
		skylight_data *data = GetInstanceEC( x>>1, z>>1 );
		
		
		if( data ) {
			int index = (x&1) + (z&1)*2;
			return data->depth + index * 16*16;
		}
		return 0;
	}

} skylight_manager;

void set_skylight_dirty( planet::context *ct, int x, int y, int z ) {
	skylight_manager.SetDirty(x,y,z);
}

typedef struct t_renderingInstance {
	//int planet_index;
	//CHUNKPTR *data_ref;
	bool registered;
	int x;
	int y;
	int z;
	int map_index;

	int vbosize;
	int vbosize_water;
	int index;			// index in rendering instance list
	int gdelay;			// rendering data update delay
	float opacity;		// opacity of cubes
 
 
	bool dirty;			// geometry has changed (assume both skylight and dimlight have changed aswell)
	bool skylight_changed; // skylight has changed
	bool dimlight_changed; // dimlight has changed

	// light data cache
	// format: 
	//  bit 0-7   = red
	//      8-15  = green
	///     16-23 = blue
	//      24-27 = sky
	// length,height = -1 to 32
	// width = -1 to 
	u32 light_cache[40*34*34];

	renderingList *parent;
	struct t_renderingInstance *prev;
	struct t_renderingInstance *next;
} renderingInstance;

typedef struct t_stamp {
	int start_vertex;
	int size;
	bool cube;
} stamp;

#define RIM_WIDTH 512
#define RIM_HEIGHT 32
u16 rendering_instance_map[RIM_WIDTH*RIM_WIDTH*RIM_HEIGHT];

int to_instance_map_index( int x, int y, int z ) {
	return x + y * RIM_WIDTH * RIM_WIDTH + z * RIM_WIDTH;
}

void set_instance_map( int map_index, int value ) {
	rendering_instance_map[map_index] = value;
}

void reset_instance_map( int map_index ) {
	set_instance_map(map_index,0);
}

void set_instance_map( int x, int y, int z, int index ) {
	set_instance_map( x + y*RIM_WIDTH*RIM_WIDTH + z*RIM_WIDTH, index );
}

void reset_instance_map( int x, int y, int z ) {
	set_instance_map(x,y,z,0);
}

int read_instance_map( int index ) {
	return rendering_instance_map[index];
}

int read_instance_map( int x, int y, int z ) {
	if( x >= 0 && x < RIM_WIDTH && z >= 0 && z < RIM_WIDTH && y >= 0 && y < RIM_HEIGHT ){
		return read_instance_map( x + y * RIM_WIDTH*RIM_WIDTH + z * RIM_WIDTH );
	}
	return 0;
}

int read_instance_map_from_point( int x, int y, int z ) {
	return read_instance_map( x>>5, y>>5, z>>5 );
}

int read_instance_map_from_point( float x, float y, float z ) {
	return read_instance_map( (int)x>>5, (int)y>>5, (int)z>>5 );
}

int read_instance_map_from_planet_index( int x, int y, int z ) {
	return read_instance_map( x>>1, y>>1, z>>1 );
}
/*
int instance_from_point( int x, int y, int z ) {
	x >>= 5;
	y >>= 5;
	z >>= 5;
	if( x >= 0 && x < RIM_WIDTH && z >= 0 && z < RIM_WIDTH && y >= 0 && y < RIM_HEIGHT ){
		return rendering_instance_map[x+y*RIM_WIDTH*RIM_HEIGHT+z*RIM_WIDTH];
	}
	return 0;
}

int instance_from_point( float x, float y, float z ) {
	return instance_from_point( (int)x, (int)y, (int)z );
}*/

Video::terrain_instance_vertex stampData[8192];
Video::terrain_instance_vertex *stampWrite = stampData;

#define MAX_STAMPS 512

stamp stamps[MAX_STAMPS];
// CUBE STAMP
// 6 VERTICES, FRONT,BACK,RIGHT,LEFT,TOP,BOTTOM

#define CONVERTUV(x) ((x)*32)

#define waterbuffer_offset (32*32*32*6)
//Video::terrain_vertex vertexBuffer[16*16*16*6*4 * 2];

Video::terrain_instance_vertex vertexBuffer[32*32*32*6*2];

bool update_light_uniforms;

//vertex waterBuffer[16*16*16*6*4];
renderingInstance instances[instanceCount];

class lightList {
private:
	lightInstance2 *first;
	lightInstance2 *last;
	
public:
	void erase() {
		first=last=0;
	}

	lightInstance2 *add() {
		lightInstance2 *i = new lightInstance2();
		if( last ) {
			i->prev = last;
			i->next = 0;
			last->next = i;
			last = i;
		} else {
			first=last=i;
			i->prev = i->next = 0;
		}
		return i;
	}
	
	void remove( lightInstance2 *i ) {
		if( i == first ) {
			first = first->next;
			if( first )
				first->prev = 0;
		} else {
			i->prev->next = i->next;
		}
		if( i == last ) {
			last = last->prev;
			if( last )
				last->next =0;
		} else {
			i->next->prev = i->prev;
		}
		delete i;
	}

	lightInstance2 *get_first() {
		return first;
	}

	lightInstance2 *get_last() {
		return last;
	}

	bool empty() {
		return !(first != 0);
	}
};

class renderingList {

private:
	renderingInstance *first;
	renderingInstance *last;

public:
	
	void erase() {
		first=last=0;
	}
	
	void add( renderingInstance *i ) {
		if( last ) {
			i->prev = last;
			i->next = 0;
			last->next = i;
			last = i;
		} else {
			first=last=i;
			i->prev = i->next = 0;
		}
		i->parent=this;
	}

	renderingInstance *remove( renderingInstance *i ) {
		if( i == first ) {
			first = first->next;
			if( first )
				first->prev = 0;
		} else {
			i->prev->next = i->next;
		}
		if( i == last ) {
			last = last->prev;
			if( last )
				last->next =0;
		} else {
			i->next->prev = i->prev;
		}
		i->prev = i->next = 0;
		return i;
	}

	renderingInstance *get_first() {
		return first;
	}

	renderingInstance *get_last() {
		return last;
	}

	bool empty() {
		return !(first != 0);
	}
};

renderingList rl_free, rl_resv, rl_active;

renderingList rl_1, rl_2, rl_3, rl_4; // distance sorted lists


lightList lights;

Video::VertexBuffer vertex_buffer;
//GLuint vboIDs[instanceCount];

enum {
	PATTERN_TEXTURE = 0,
	PATTERN_FORM = 4,
	PATTERN_NORMAL = 8,
	PATTERN_MT_COORDINATES = 12,
	PATTERN_MT_DIMENSIONS = 13
};

enum {
	MT_SELX, MT_SELY, MT_SELZ
};

enum {
	FORM_CUBE_FRONT,
	FORM_CUBE_BACK,
	FORM_CUBE_RIGHT,
	FORM_CUBE_LEFT,
	FORM_CUBE_TOP,
	FORM_CUBE_BOTTOM,
	FORM_CROSS1,
	FORM_CROSS2
};

enum {
	TEXTURE_WATER = 256
};

void add_pattern_data( int type, int index, int width, const float *data ) {
	
	// index = 0-8191
	// data = 4x4 floats (rgba x4)
	Textures::Bind( Textures::TERRAIN_PATTERNS );
	Textures::SubImageTextureF32( Textures::TERRAIN_PATTERNS, type, index, width, 1, data );
}

void copy_pattern_data4x( int type, int index, const cml::vector4f *data ) {
	float fdata[4*4];

	// convert data
	for( int i = 0; i < 4; i++ ) {
		for( int j = 0; j < 4; j++ ) {
			
			fdata[i*4+j] = data[i][j];
			
		}
	}

	add_pattern_data( type, index, 4, fdata );
}

void copy_pattern_data1x( int type, int index, const cml::vector4f &data ) {
	float fdata[4];

	// convert data
	for( int i = 0; i < 4; i++ ) {
		
		fdata[i] = data[i];
			
	}

	add_pattern_data( type, index, 1, fdata );
}

void set_form_pattern( int index, const cml::vector3f *verts ) {
	cml::vector4f data[4];
	for( int i = 0; i < 4; i++ )
		data[i] = cml::vector4f( verts[i][0], verts[i][1], verts[i][2], 0.0f );
	
	copy_pattern_data4x( PATTERN_FORM, index, data );
}

void set_normal_pattern( int index, const cml::vector3f *normal ) {
	cml::vector4f data[4];
	for( int i = 0; i < 4; i++ )
		data[i] = cml::vector4f( normal[i][0], normal[i][1], normal[i][2], 0.0f );
	
	copy_pattern_data4x( PATTERN_NORMAL, index, data );
}

void set_texture_pattern( int index, const cml::vector3f *uvw ) {
	cml::vector4f data[4];
	for( int i = 0; i < 4; i++ )
		data[i] = cml::vector4f( uvw[i][0], uvw[i][1], uvw[i][2], 0.0f );
	
	copy_pattern_data4x( PATTERN_TEXTURE, index, data );
}

void set_metatexture_coordinates_pattern( int index, const cml::vector2f &ab ) {
	cml::vector4f dat( ab[0], ab[1], 0, 0 );
	copy_pattern_data1x( PATTERN_MT_COORDINATES, index, dat );
}

void set_metatexture_dimensions_pattern( int index, const cml::vector2f &ab ) {
	cml::vector4f dat( ab[0], ab[1], 0, 0 );
	copy_pattern_data1x( PATTERN_MT_DIMENSIONS, index, dat );
}

void set_stamp_vertex( Video::terrain_instance_vertex *v, unsigned short form, unsigned short texture ) {
	v->form = form;
	v->texture = texture;
}

int get_stamp_vertex_index( Video::terrain_instance_vertex *v ) {
	return ((unsigned int)v-(unsigned int)stampData) / sizeof(Video::terrain_instance_vertex);
}
 
stamp * set_empty_stamp( int index ) {
	stamps[index].start_vertex = stamps[index].size = 0;
	stamps[index].cube = false;
	return stamps + index;
}
 
void add_stamp_vertex( int form, int texture ) {
	stampWrite->form = form;
	stampWrite->texture = texture;
	stampWrite++;
}

void set_cube_stamp( int index, int front, int back, int right, int left, int top, int bottom ) {
	stamps[index].start_vertex = ((unsigned int)stampWrite-(unsigned int)stampData) / sizeof(Video::terrain_instance_vertex);
	stamps[index].size = 6;
	stamps[index].cube = true;
	
	set_stamp_vertex( stampWrite++, FORM_CUBE_FRONT, front );
	set_stamp_vertex( stampWrite++, FORM_CUBE_BACK, back );
	set_stamp_vertex( stampWrite++, FORM_CUBE_RIGHT, right );
	set_stamp_vertex( stampWrite++, FORM_CUBE_LEFT, left );
	set_stamp_vertex( stampWrite++, FORM_CUBE_TOP, top );
	set_stamp_vertex( stampWrite++, FORM_CUBE_BOTTOM, bottom );
}
 
__forceinline void copy_cube_face_i( const stamp *source, Video::terrain_instance_vertex *write, int x, int y, int z, int index, __m128i vertex_color ) {
	const Video::terrain_instance_vertex *sv = stampData + source->start_vertex + index;
	write->translation = x + (y<<10) + (z<<5);
	write->form = sv->form;
	write->texture = sv->texture;
	_mm_storeu_si128( (__m128i*)write->colors, vertex_color );
}
 
__forceinline Video::terrain_instance_vertex * copy_stamp_full( const stamp *source, Video::terrain_instance_vertex *write, int x, int y, int z, __m128i vertex_color ) {
	const Video::terrain_instance_vertex *sv = stampData + source->start_vertex;
	for( int i = 0; i < source->size; i++ ) {
		write->translation = x + (y<<10) + (z<<5);
		write->form = sv[i].form;
		write->texture = sv[i].texture;
		_mm_storeu_si128( (__m128i*)write->colors, vertex_color );
		write++;
	}
	return write;
}
 
#define ambience 0.05
  

//#define STEPC2 324
//#define STEPF2 1764
#define STEPCZ 40 // FOR 1-BORDER BUFFERS 40X34X34 (X-AXIS PADDED)
#define STEPCY (40*34)
#define STEPFZ 64 // FOR 16-BORDER BUFFERS 48X48X48
#define STEPFY (64*64)
#define TOTALC (40*34*34)
#define TOTALF (64*64*64)

// note: bg = "build geometry"

_CRT_ALIGN(16) u8 bg_data_buffer[TOTALF];
_CRT_ALIGN(16) u32 bg_light_buffer[TOTALC+16]; // 2.0

//_CRT_ALIGN(16) u8 bg_lightstamp_buffer_cropped[STEPC*STEPC*STEPC+32];
_CRT_ALIGN(16) u8 bg_lightstamp_buffer[TOTALF+32];


#define INDEXC(x,y,z) ((x)+((y)*STEPCY)+((z)*STEPCZ))
#define INDEXF(x,y,z) ((x)+((y)*STEPFY)+((z)*STEPFZ))

const u8 lum_map[] = {
	0,0,0,0, 0,0,0,0, 10,10,10,10, 0,0,0,0, 
	0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 
	0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 
	0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 
	
	0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 
	0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 
	0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 
	0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 
	
	0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 
	0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 
	0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 
	0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 
	
	0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 
	0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 
	0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 
	0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0};

const u8 red_map[] = {
	0,0,0,0, 0,0,0,0, 255,248,124,0, 0,0,0,0, 
	0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 
	0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 
	0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 
	
	0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 
	0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 
	0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 
	0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 
	
	0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 
	0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 
	0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 
	0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 
	
	0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 
	0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 
	0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 
	0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0};

const u8 green_map[] = {
	0,0,0,0, 0,0,0,0, 255,148,197,174, 0,0,0,0, 
	0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 
	0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 
	0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 
	
	0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 
	0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 
	0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 
	0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 
	
	0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 
	0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 
	0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 
	0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 
	
	0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 
	0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 
	0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 
	0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0};

const u8 blue_map[] = {
	0,0,0,0, 0,0,0,0, 255,29,118,240, 0,0,0,0, 
	0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 
	0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 
	0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 
	
	0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 
	0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 
	0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 
	0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 
	
	0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 
	0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 
	0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 
	0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 
	
	0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 
	0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 
	0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 
	0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0};

const u8 opacity_map[] = {
	0xf,0xf,0xf,0xf, 0xf,0xf,0xf,0xf, 0xf,0xf,0xf,0xf, 0,1,2,3, 
	0xf,0xf,0xf,0xf, 0xf,0xf,0xf,0xf, 0xf,0xf,0xf,0xf, 0,1,2,3, 
	0xf,0xf,0xf,0xf, 0xf,0xf,0xf,0xf, 0xf,0xf,0xf,0xf, 0,1,2,3, 
	0xf,0xf,0xf,0xf, 0xf,0xf,0xf,0xf, 0xf,0xf,0xf,0xf, 0,1,2,3, 
	
	0xf,0xf,0xf,0xf, 0xf,0xf,0xf,0xf, 0xf,0xf,0xf,0xf, 0,1,2,3, 
	0xf,0xf,0xf,0xf, 0xf,0xf,0xf,0xf, 0xf,0xf,0xf,0xf, 0,1,2,3, 
	0xf,0xf,0xf,0xf, 0xf,0xf,0xf,0xf, 0xf,0xf,0xf,0xf, 0,1,2,3, 
	0xf,0xf,0xf,0xf, 0xf,0xf,0xf,0xf, 0xf,0xf,0xf,0xf, 0,1,2,3, 
	
	0xf,0xf,0xf,0xf, 0xf,0xf,0xf,0xf, 0xf,0xf,0xf,0xf, 0,1,2,3, 
	0xf,0xf,0xf,0xf, 0xf,0xf,0xf,0xf, 0xf,0xf,0xf,0xf, 0,1,2,3, 
	0xf,0xf,0xf,0xf, 0xf,0xf,0xf,0xf, 0xf,0xf,0xf,0xf, 0,1,2,3, 
	0xf,0xf,0xf,0xf, 0xf,0xf,0xf,0xf, 0xf,0xf,0xf,0xf, 0,1,2,3, 
	
	0xf,0xf,0xf,0xf, 0xf,0xf,0xf,0xf, 0xf,0xf,0xf,0xf, 0,1,2,3, 
	0xf,0xf,0xf,0xf, 0xf,0xf,0xf,0xf, 0xf,0xf,0xf,0xf, 0,1,2,3, 
	0xf,0xf,0xf,0xf, 0xf,0xf,0xf,0xf, 0xf,0xf,0xf,0xf, 0,1,2,3, 
	0xf,0xf,0xf,0xf, 0xf,0xf,0xf,0xf, 0xf,0xf,0xf,0xf, 0,1,2,3, 
};

//typedef void (*copy_data_function)( CHUNKPTR s, int x, int y, int z, int w, int h, int d, int tx, int ty, int tz );

/*
void copy_chunk_region_data( CHUNKPTR s, int x, int y, int z, int w, int h, int d, int tx, int ty, int tz ) {
	if( planet::real_chunk(s) ) { // copy chunk data without bound testing
		planet::chunk *ch = planet::get_chunk(s);
		for( int dy = 0; dy < h; dy++ ) { for( int dz = 0; dz < d; dz++ ) { int dx;
			for( dx = 0; dx < w-3; dx+=4 ) {
				*((u32*)&bg_data_buffer[INDEXF(tx+dx,ty+dy,tz+dz)]) = *((u32*)&ch->data[x+dx + (y+dy)*256 + (z+dz)*16]); 
			} for( ; dx < w; dx++ ) {
				bg_data_buffer[INDEXF(tx+dx,ty+dy,tz+dz)] = ch->data[x+dx + (y+dy)*256 + (z+dz)*16]; 
		}}}
	} else {
		int a = planet::read_state(s) == planet::CHUNK_EMPTY ? planet::CELL_AIR : planet::CELL_ROCK;
		for( int dy = 0; dy < h; dy++ ) { for( int dz = 0; dz < d; dz++ ) { for( int dx = 0; dx < w; dx++ ) {
					bg_data_buffer[INDEXF(tx+dx,ty+dy,tz+dz)] = a;
		}}}
	}
}*/
/*
void copy_chunk_region_skylight( CHUNKPTR s, int x, int y, int z, int w, int h, int d, int tx, int ty, int tz ) {
	if( planet::real_chunk(s) ) { // copy chunk data without bound testing
		planet::chunk *ch = planet::get_chunk(s);
		for( int dy = 0; dy < h; dy++ ) { for( int dz = 0; dz < d; dz++ ) { int dx;
			for( dx = 0; dx < w-3; dx+=4 ) {
				*((u32*)&bg_lightstamp_buffer[INDEXF(tx+dx,ty+dy,tz+dz)]) = *((u32*)&ch->sunlight[x+dx + (y+dy)*256 + (z+dz)*16]); 
			} for( ; dx < w; dx++ ) {
				bg_lightstamp_buffer[INDEXF(tx+dx,ty+dy,tz+dz)] = ch->sunlight[x+dx + (y+dy)*256 + (z+dz)*16]; 
		}}}
	} else {
		int light = planet::read_state(s) == planet::CHUNK_EMPTY ? 12 : 0;
		for( int dy = 0; dy < h; dy++ ) { for( int dz = 0; dz < d; dz++ ) { for( int dx = 0; dx < w; dx++ ) {
					bg_lightstamp_buffer[INDEXF(tx+dx,ty+dy,tz+dz)] = light;
		}}}
	}
}
*/
/*
void copy_region_data( int ox, int oy, int oz, int x1, int y1, int z1, int x2, int y2, int z2, copy_data_function function ) {
	// ox,oy,oz = buffer origin [at 13,13,13]
	for( int y = y1; y <= y2; y++ ) {
		for( int z = z1; z <= z2; z++ ) {
			for( int x = x1; x <= x2; x++ ) {
				CHUNKPTR s = planet::get_chunk_pointer( (ox+x)>>4 , (oy+y)>>4 , (oz+z)>>4 );
				int w = 15 - (x&15);
				int h = 15 - (y&15);
				int d = 15 - (z&15);
				if( (x+w) > x2 ) w = x2-x;
				if( (y+h) > y2 ) h = y2-y;
				if( (z+d) > z2 ) d = z2-z;
				function( s, x&15, y&15, z&15, w + 1, h + 1, d + 1, 12+1+x, 12+1+y, 12+1+z );
				x |= 15;
			}
			z |= 15;
		}
		y |= 15;
	}
}*/






void lightbeam_x ( int index, int v );
void lightbeam_y ( int index, int v );
void lightbeam_z ( int index, int v ); 

__forceinline void lightbeam_down( int index, int v ) {
	index -= STEPFY;
	for( ;; ) {
		v -= opacity_map[bg_data_buffer[index]] + 1; // todo: set in opacity map instead of +1
		//if( v <= 0 ) return;
		if( v > bg_lightstamp_buffer[index] ) { bg_lightstamp_buffer[index] = v; }
		else return;

		lightbeam_x( index, v ); 
		lightbeam_z( index, v );
		
		index -= STEPFY;
}	}
void lightbeam_y( int index, int v ) {
	lightbeam_down(index,v);
	index += STEPFY;
	for( ;; ) {
		v -= opacity_map[bg_data_buffer[index]] + 1;
		//if( v <= 0 ) return;
		if( v > bg_lightstamp_buffer[index] ) { bg_lightstamp_buffer[index] = v; }
		else return;

		lightbeam_x( index, v ); 
		lightbeam_z( index, v ); 
			
		index += STEPFY;
}	}
 
__forceinline void lightbeam_left( int index, int v ) {
	index--;
	for( ;; ) {
		v -= opacity_map[bg_data_buffer[index]] + 1;
		//if( v <= 0 ) return;
		if( v > bg_lightstamp_buffer[index] ) { bg_lightstamp_buffer[index] = v; }
		else return;

		lightbeam_y( index, v ); 
		lightbeam_z( index, v ); 
			
		index--;	
}	}
void lightbeam_x( int index, int v ) {
	lightbeam_left( index, v );
	index++;
	for( ;; ) {
		v -= opacity_map[bg_data_buffer[index]] + 1;
		//if( v <= 0 ) return;
		if( v > bg_lightstamp_buffer[index] ) { bg_lightstamp_buffer[index] = v; }
		else return;

		lightbeam_y( index, v ); 
		lightbeam_z( index, v ); 
			
		index++;	
}	}

__forceinline void lightbeam_back( int index, int v ) {
	index -= STEPFZ;
	for( ;; ) {
		v -= opacity_map[bg_data_buffer[index]] + 1;
		//if( v <= 0 ) return;
		if( v > bg_lightstamp_buffer[index] ) { bg_lightstamp_buffer[index] = v; }
		else return;

		lightbeam_y( index, v ); 
		lightbeam_x( index, v ); 
			
		index -= STEPFZ;
}	}
void lightbeam_z( int index, int v ) {
	lightbeam_back(index,v);
	index += STEPFZ;
	for( ;; ) {
		v -= opacity_map[bg_data_buffer[index]] +1;
		//if( v <= 0 ) return;
		if( v > bg_lightstamp_buffer[index] ) { bg_lightstamp_buffer[index] = v; }
		else return;

		lightbeam_y( index, v );
		lightbeam_x( index, v );
		
		index += STEPFZ;
}	}


void add_ambient_light() {
	// for each light cache CELL:
	// if corresponding DATA is an 'empty' block:
	//   CELL = AMBIENT
	// else
	//   CELL = ZERO
	
	u32 ambient_light = 0x00050505; // raw light 5/256

	u32 *dest = bg_light_buffer;
	u8 *source;
	

	for( int y = 0; y < 34; y++ ) {
		for( int z = 0; z < 34; z++ ) {

			source = (u8*)&bg_data_buffer[INDEXF(12,15+y,15+z)];

			for( int i = 0; i < 10; i++ ) {

				__m128i stamp = _mm_setzero_si128();
			
				u8 source_byte = *source++;
				if( source_byte == 12 ) stamp = _mm_insert_epi32( stamp, ambient_light, 0 );
				source_byte = *source++;
				if( source_byte == 12 ) stamp = _mm_insert_epi32( stamp, ambient_light, 1 );
				source_byte = *source++;
				if( source_byte == 12 ) stamp = _mm_insert_epi32( stamp, ambient_light, 2 );
				source_byte = *source++;
				if( source_byte == 12 ) stamp = _mm_insert_epi32( stamp, ambient_light, 3 );

				__m128i b = _mm_load_si128( (__m128i*)dest ); // b = 4 light buffer entries (dwords)
				b = _mm_adds_epu8( b, stamp ); // add ambient light bytes with saturation (4x4 bytes)
				_mm_store_si128( (__m128i*)dest, b ); // store result

				dest += 4;

			}
		}
	}
}

void save_light_cache( renderingInstance *ri ) {
	u32 *source = bg_light_buffer;
	u32 *dest = ri->light_cache;
	for( int i = 0; i < TOTALC/4; i++ ) {
		__m128i a = _mm_load_si128( (__m128i*)source );
		_mm_storeu_si128( (__m128i*)dest, a );
		source += 4;
		dest += 4;
	}

}

void copy_light_cache_direct( renderingInstance *ri ) {
	u32 *source = ri->light_cache;
	u32 *dest = bg_light_buffer;
	for( int i = 0; i < TOTALC/4; i++ ) {
		__m128i a = _mm_loadu_si128( (__m128i*)source );
		_mm_store_si128( (__m128i*)dest, a );
		source += 4;
		dest += 4;
	}
}

void copy_skylight_cache_reset_dimlight( renderingInstance *ri ) {

	u32 *source = (u32*)ri->light_cache;
	u32 *dest = bg_light_buffer;

	for( int i = 0; i < TOTALC; i+=4 ) {
		
		// read light cache
		__m128i a = _mm_loadu_si128( (__m128i*)source );

		// mask out dimlight components
		a = _mm_srli_epi32( a, 24 );
		a = _mm_slli_epi32( a, 24 );


		_mm_store_si128( (__m128i*)dest, a );
		
		source += 4;
		dest += 4;
	}
}

void copy_and_add_dimlight_cache( renderingInstance *ri ) {

	u32 *source = (u32*)ri->light_cache;
	u32 *dest = bg_light_buffer;

	for( int i = 0; i < TOTALC; i+=4 ) {
		
		// read light cache
		__m128i a = _mm_loadu_si128( (__m128i*)source );

		// mask out skylight component
		a = _mm_slli_epi32( a, 8 );
		a = _mm_srli_epi32( a, 8 );

		// read light buffer
		__m128i b = _mm_load_si128( (__m128i*)dest );

		// assume lower 3 components of light buffer are zero
		// apply dimlight cache
		a = _mm_or_si128( a, b );

		_mm_store_si128( (__m128i*)dest, a );
		
		source += 4;
		dest += 4;
	}
}

void spread_skylight( int sector ) {

	int basex,basey,basez;
	basex = (sector&3);
	basey = ((sector>>4)&3);
	basez = ((sector>>2)&3);

	int base = INDEXF(basex*16,basey*16,basez*16);

	if( bg_data_buffer[base] == planet::CELL_RENDERVOID ) return;

	int start[3],end[3];
	for( int i = 0; i < 3; i++ ) {
		start[i] = 0;
		end[i] = 15;
	}

	if( basex == 0 ) start[0] = 1;
	if( basex == 3 ) end[0] = 14;
	if( basez == 0 ) start[1] = 1;
	if( basez == 3 ) end[1] = 14;
	if( basey == 0 ) start[2] = 1;
	if( basey == 3 ) end[2] = 14;
	


	for( int y = start[2]; y <= end[2]; y+=2 ) {
		for( int z = start[1]; z <= end[1]; z+=2 ) {
			for( int x = start[0]; x < end[0]; x+=2 ) {
				int index = base+INDEXF( x + ((y&2) ? 1:0),y,z + ((y&4) ? 1:0) );
				
				int v = bg_lightstamp_buffer[index];
				
				
				if( v > 1 && v != 255 ) {

					lightbeam_x( index, v ); 
					lightbeam_y( index, v ); 
					lightbeam_z( index, v ); 
				}
				
			}
		}
	}
}

void reset_light_stamp() {
	__m128i a = _mm_setzero_si128();

	u8 *dest = bg_lightstamp_buffer;
	for( int i = 0; i < TOTALF/16; i++ ) {
		_mm_store_si128( (__m128i*)dest, a );
		dest += 16;
	}
}
 

void set_light_buffer_with_skylight() {
	// fill cache with skystamp
	// each dword entry = [SKY][0][0][0]
	u32 *dest = bg_light_buffer;
	u32 *source;

	__m128i shuffle = _mm_set_epi32( 0x03808080, 0x02808080, 0x01808080, 0x00808080);

	// each light entry (dword) = source entry (byte) shifted left 12

	for( int y = 0; y < 34; y++ ) {
		for( int z = 0; z < 34; z++ ) {
			source = (u32*)&bg_lightstamp_buffer[INDEXF(12,15+y,15+z)];

			// operation:
			// read dword from light buffer (4 bytes)
			// swizzle data so each byte is the upper byte of 4 dwords
			// add ambient setting (?) todo?
			for( int i = 0; i < 10; i++ ) {
				__m128i a = _mm_cvtsi32_si128( *source++ );
				a = _mm_shuffle_epi8( a, shuffle );
				_mm_store_si128( (__m128i*)dest, a );
				dest += 4;
			}
		}
	}
}

void make_light_stamp( int index, int v ) {
	bg_lightstamp_buffer[index] = v;
	if( v > 1 ) {
		
		lightbeam_x( index, v );
		lightbeam_y( index, v );
		lightbeam_z( index, v );
	}
}

void apply_dimlight_stamp( int r, int g, int b ) {

	u32 *dest = bg_light_buffer;
	u32 *source;
	
	// r,g,b = 0-255
	r>>=2;
	g>>=2;
	b>>=2;

	__m128i color = _mm_set_epi16( 0,  b,  g,  r,  0,  b,  g,  r );
	__m128i shuffle1 = _mm_set_epi16( (short)0x8001, (short)0x8001, (short)0x8001, (short)0x8001, (short)0x8000, (short)0x8000, (short)0x8000, (short)0x8000 );
	__m128i shuffle2 = _mm_set_epi16( (short)0x8003, (short)0x8003, (short)0x8003, (short)0x8003, (short)0x8002, (short)0x8002, (short)0x8002, (short)0x8002 );

	// for each entries in light stamp (in the cache window)
	// multiply by color constant
	// pack into 8bit x3
	// add with saturation to light cache

	for( int y = 0; y < 34; y++ ) {
		for( int z = 0; z < 34; z++ ) {

			source = (u32*)&bg_lightstamp_buffer[INDEXF(12,15+y,15+z)];

			for( int i = 0; i < 10; i++ ) {
			
				// load 4 bytes of lightstamp
				__m128i a = _mm_cvtsi32_si128( *source++ );
				// a = d c b a (bytes)

				// unpack data
				__m128i b = _mm_shuffle_epi8( a, shuffle1 ); // b = b b b b a a a a (words)
				__m128i c = _mm_shuffle_epi8( a, shuffle2 ); // c = d d d d c c c c (words)
			
				// multiply by color
				// (4bit) * (8bit) = (12bit)
				b = _mm_mullo_epi16( b, color ); // b = 0bgr(1) 0bgr(0) x16
				c = _mm_mullo_epi16( c, color ); // c = 0bgr(3) 0bgr(2) x16

				b = _mm_srli_epi16( b, 4 );  // shift to 8bit
				c = _mm_srli_epi16( c, 4 );
			
				a = _mm_packus_epi16( b, c ); // pack,saturate,and concatenate result into 'a'
				// a = 0bgr0bgr0bgr0bgr
				///    ddddccccbbbbaaaa

				b = _mm_load_si128( (__m128i*)dest ); // b = 4 light buffer entries (dwords)
				b = _mm_adds_epu8( b, a ); // add light bytes with saturation (4x4 bytes)
				_mm_store_si128( (__m128i*)dest, b ); // store result

				dest += 4;

				// repeat 4 more times
				// (stride = 20 dwords)
			}
			
		}
	}
}

void spread_dimlight( int sector ) {
	
	// todo: clipping border shapes
	int base = INDEXF((sector&3)*16,((sector>>4)&3)*16,((sector>>2)&3)*16);
	if( bg_data_buffer[sector] == planet::CELL_RENDERVOID ) return;

	for( int y = 0; y <= 15; y++ ) {
		for( int z = 0; z <= 15; z++)  {
			for( int x = 0; x <= 15; x++ ) {
				int index = base + INDEXF(x,y,z);
				int v = bg_data_buffer[index];
				if( lum_map[v] != 0 ) {
					reset_light_stamp();
					make_light_stamp( index, lum_map[v] );
					apply_dimlight_stamp( red_map[v], green_map[v], blue_map[v] );
				}
			}
		}
	}
}

cml::vector4f compute_light( float x, float y, float z ) {
	//CHUNKPTR s = planet::get_chunk_pointer((int)x>>4,(int)y>>4,(int)z>>4);

	cml::vector4f light;

	//planet::chunk *ch = planet::get_chunk_direct(x>>4,y>>4,z>>4);
	int index  = read_instance_map_from_point( x, y, z );

	/*if( planet::real_chunk(s) ) {
		planet::chunk *ch = planet::get_chunk(s);
		index = ch->instance;
	}*/
	
	if( index > 0 ) {
		u32 l32 = instances[index-1].light_cache[INDEXC( ((4+((int)x&31))), ((1+((int)y&31))), ((1+((int)z&31))) )];
		u8 r = (l32);
		u8 g = ((l32) >> 8);
		u8 b = ((l32) >> 16);
		u8 s = ((l32) >> 24);
		light[0] = (float)r * (1.0f/63.0f);
		light[1] = (float)g * (1.0f/63.0f);
		light[2] = (float)b * (1.0f/63.0f);
		light[3] = (float)s * (1.0f/15.0f);

	} else {
		light[0] = 0.0f;
		light[1] = 0.0f;
		light[2] = 0.0f;
		if( y < planet::get_sea_level() ) {
			
			light[3] = 0.0f;
		} else {
			light[3] = 1.0f;	
		}
	}

	for( lightInstance2 *l = lights.get_first(); l; l = l->next ) {
		// must match shader code:
		float dvx,dvy,dvz;
		dvx = l->x - x;
		dvy = l->y - y;
		dvz = l->z - z;
		float dist = dvx*dvx + dvy*dvy + dvz*dvz;
		if( dist < l->brightness * l->brightness ) {
			dist = sqrt(dist);
			
			float intensity = 1.0f - (dist / l->brightness);
			intensity *= intensity;
			if( intensity > 0.0 ) {
				light[0] += l->r * intensity;
				light[1] += l->g * intensity;
				light[2] += l->b * intensity;
				
			}
		}
	}
	return light;
}

u32 compute_light_7bit( float x, float y, float z ) {
	cml::vector4f light = compute_light(x,y,z);

	int r = (int)(light[0] * 128.0f);
	int g = (int)(light[1] * 128.0f);
	int b = (int)(light[2] * 128.0f);
	apply_sunlight(r,g,b,(int)(light[3] ));
	r = r > 255 ? 255 : r;
	g = g > 255 ? 255 : g;
	b = b > 255 ? 255 : b;
	return (r)+(g<<8)+(b<<16);
}

/*
u32 get_light_value( int x, int y, int z ) {
	CHUNKPTR s = planet::get_chunk_pointer(x>>4,y>>4,z>>4);
	//planet::chunk *ch = planet::get_chunk_direct(x>>4,y>>4,z>>4);
	if( planet::real_chunk(s) ) {
		planet::chunk *ch = planet::get_chunk(s);
		int index = ch->instance;
		if( index > 0 ) {
			
			return instances[index-1].light_cache[INDEXC( ((1+(x&15))), ((1+(y&15))), ((1+(z&15))) )];
		} else {
			return 0x0C000000;
		}
	} else {
		return 0x0C000000;
	}
}*/

renderingInstance* get_instance( int index ) {
	return instances+index;
}
/*
u8 cell_is_prop[] = {
	0,0,0,0, 0,0,0,0, 0,0,0,0, 0,1,0,0, 
	0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 
	0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 
	0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 
	
	0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 
	0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 
	0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 
	0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 
	
	0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 
	0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 
	0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 
	0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 
	
	0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 
	0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 
	0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 
	0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 
};*/

u16 empty_skylight_data[16*16];

void copy_skylight_slice( u16 *data, int tx, int tz, int bottom_y ) {
	u8 *target_start = (u8*)(bg_lightstamp_buffer + tx * 16 + tz * 16 * STEPFZ);
	
	for( int z = 0; z < 16; z++ ) {

		for( int x = 0; x < 16; x++ ) {

			u8 *target = target_start + x + z * STEPFZ;
			u16 depth = data[x+z*16];

			for( int y = 0; y < 64; y++ ) {
				*target = ( (bottom_y+y) >= depth ) ? 15 : 0;
				target += STEPFY;
			}
		}
	}
}

void initialize_skylight_stamp( int x, int y, int z ) {

	int bottom_y = y*16-16;
	
	for( int cz = 0; cz <= 3; cz++ ) {
		for( int cx = 0; cx <= 3; cx++ ) {
			u16 *data;
			data = skylight_manager.GetData( x-1+cx, z-1+cz );
			
			
			if( !data ) {
//				assert(false);
				data = empty_skylight_data;
			} else {
				data =data;
			}
			copy_skylight_slice( data, cx, cz, bottom_y );
			
		}
	}
	
}

void copy_data_chunk( planet::chunk *ch, int tx, int ty, int tz ) {
	// copy chunk from(x,y,z) to buffer(tx,ty,tz)

	__m128i a;

	u8 *target_start = (u8*)(bg_data_buffer + tx * 16 + ty * 16 * STEPFY + tz * 16 * STEPFZ);
	u8 *source = (u8*)(ch->data);

	for( int y = 0; y < 16; y++ ) {
		u8 *target = target_start + y * STEPFY;
		
		for( int z = 0; z < 16; z++ ) {
			a = _mm_load_si128( (__m128i*)source );
			_mm_store_si128( (__m128i*)target, a );
			source += 16;
			target += STEPFZ;
		}

	}
}

void erase_data_chunk( int tx, int ty, int tz ) {
	__m128i a = _mm_setzero_si128();
	int cell = planet::CELL_RENDERVOID;
	if( tx >= 1 && tx <= 2 && ty >= 1 && ty <= 2 && tz >= 1 && tz <= 2 ) {
		cell = planet::CELL_AIR;
	}

	a = _mm_set_epi32( 
		cell | (cell<<8) | (cell<<16) | (cell<<24),
		cell | (cell<<8) | (cell<<16) | (cell<<24),
		cell | (cell<<8) | (cell<<16) | (cell<<24),
		cell | (cell<<8) | (cell<<16) | (cell<<24) );

	u8 *target = (u8*)(bg_data_buffer + tx * 16 + ty * 16 * STEPFY + tz * 16 * STEPFZ);

	for( int y = 0; y < 16; y++ ) {
		for( int z = 0; z < 16; z++ ) {
			_mm_store_si128( (__m128i*)(target + z * STEPFZ + y * STEPFY), a );
		}
	}
}

void initialize_data_buffer( int x, int y, int z ) {
	for( int cy = 0; cy <= 3; cy++ ) {
		for( int cz = 0; cz <= 3; cz++ ) {
			for( int cx = 0; cx <= 3; cx++ ) {
				planet::chunk *ch = planet::get_chunk( planet::get_chunk_pointer(x+cx-1,y+cy-1,z+cz-1) );
				if( ch ) {
					copy_data_chunk( ch, cx, cy, cz );
				} else {
					erase_data_chunk( cx, cy, cz );
				}
			}
		}
	}
}

void set_safety_border() {
	for( int x = 0; x < 64; x++ ) { for( int y = 0; y < 64; y++ ) {
		bg_data_buffer[x+y*STEPFZ] = planet::CELL_ROCK;				// bottom
		bg_data_buffer[x+y*STEPFZ+63*STEPFY] = planet::CELL_ROCK;	// top
		
	}}

	for( int x = 0; x < 64; x++ ) { for( int y = 0; y < 64; y++ ) {
		bg_data_buffer[x+y*STEPFY] = planet::CELL_ROCK;				// back
		bg_data_buffer[x+y*STEPFY+63*STEPFZ] = planet::CELL_ROCK;	// front

		bg_data_buffer[x*STEPFZ+y*STEPFY] = planet::CELL_ROCK;		// left
		bg_data_buffer[x*STEPFZ+y*STEPFY+63] = planet::CELL_ROCK;	// right
	}}
}

void buildGeometry2( renderingInstance *ri ) {

	Video::terrain_instance_vertex *write = vertexBuffer;
	Video::terrain_instance_vertex *write_water = vertexBuffer+waterbuffer_offset;
	Video::terrain_instance_vertex *water_start = write_water;

	/// all-water test
	//  ^ (huh? todo?)
	 
	// copy region data into work buffers
	// bigger data used for spreading light/skylight
	//
	initialize_data_buffer(ri->x*2,ri->y*2,ri->z*2);
	if( ri->skylight_changed || ri->dimlight_changed ) {
		set_safety_border();
	}
	

	// do lighting work
	if( ri->skylight_changed ) {
		initialize_skylight_stamp( ri->x*2, ri->y*2, ri->z*2 );
		for( int i = 0; i < 64; i++ ) spread_skylight(i);

		if( ri->dimlight_changed ) {
			set_light_buffer_with_skylight();
			add_ambient_light();
			for( int i = 0; i < 64; i++ ) spread_dimlight(i);
			
		} else {
			set_light_buffer_with_skylight();
			add_ambient_light();
		 
			copy_and_add_dimlight_cache( ri );
		}
		save_light_cache( ri );
	} else {
		if( ri->dimlight_changed ) {
			copy_skylight_cache_reset_dimlight(ri);
			add_ambient_light();
			for( int i = 0; i < 64; i++ ) spread_dimlight(i);
			save_light_cache( ri );
		} else {
			copy_light_cache_direct(ri);
		}
	}

	// light buffer is now populated

	// clear dirty flags
	ri->dirty = 0;
	ri->skylight_changed = 0;
	ri->dimlight_changed = 0;

	
	// front, back, right, left, top, bottom
	
	

	for( int y = 0; y < 32; y++ ) {
		for( int z = 0; z < 32; z++ ) {
			for( int x = 0; x < 32; x++ ) {
				int ixf = INDEXF(16+x,16+y,16+z);
				int ixc = INDEXC(4+x,1+y,1+z);

				int c = bg_data_buffer[ixf];

				if( c != planet::CELL_AIR ) { // air test

					const stamp *s = stamps + c;

					//if( source->solid & planet::CHUNK_SOLID_BOTTOM )
					//	s = stamps+1;

					if( !s->cube ) {

						// easyness
						__m128i color = _mm_cvtsi32_si128(bg_light_buffer[ixc]);
						color = _mm_shuffle_epi32( color, _MM_SHUFFLE( 0, 0, 0, 0 ) );
						// color = 4 dword copies of 1 light buffer entry

						/*
						int sky = bg_sunlight_buffer[INDEXF(13+x,13+y,13+z)];
						int r,g,b;
						r = bg_r_buffer_sat[INDEXC(1+x,1+y,1+z)];
						g = bg_g_buffer_sat[INDEXC(1+x,1+y,1+z)];
						b = bg_b_buffer_sat[INDEXC(1+x,1+y,1+z)];
						*/
						write = copy_stamp_full( s, write, x, y, z, color );

						//copy_stamp( s, write, x + ri->x*16, y + ri->y*16, z + ri->z*16, sun/15.0,r,g,b);
						//write += s->size;
					} else {

	//#define UNSOLID_TILE_END 3

//#define maxlight 15.0f
//#define sunscale 15.0
						int mask = 0;
						
						if( ((bg_data_buffer[ixf+STEPFZ]) &0xf) >= 12 ) mask |= 1; // front
						if( ((bg_data_buffer[ixf-STEPFZ]) &0xf) >= 12 ) mask |= 2; // back
						if( ((bg_data_buffer[ixf+1])      &0xf) >= 12 ) mask |= 4; // right
						if( ((bg_data_buffer[ixf-1])      &0xf) >= 12 ) mask |= 8; // left
						if( ((bg_data_buffer[ixf+STEPFY]) &0xf) >= 12 ) mask |= 16; // top
						if( ((bg_data_buffer[ixf-STEPFY]) &0xf) >= 12 ) mask |= 32; // bottom

						Video::terrain_instance_vertex **target = &write;

						if( c == planet::CELL_WATER ) { // special
							if( (bg_data_buffer[ixf+STEPFZ]) == planet::CELL_WATER ) mask &= ~1; // front
							if( (bg_data_buffer[ixf-STEPFZ]) == planet::CELL_WATER ) mask &= ~2; // back
							if( (bg_data_buffer[ixf+1])      == planet::CELL_WATER ) mask &= ~4; // right
							if( (bg_data_buffer[ixf-1])      == planet::CELL_WATER ) mask &= ~8; // left
							if( (bg_data_buffer[ixf+STEPFY]) == planet::CELL_WATER ) mask &= ~16; // top
							if( (bg_data_buffer[ixf-STEPFY]) == planet::CELL_WATER ) mask &= ~32; // bottom
							target = &write_water;
						}
						 

						__m128i color1;
						__m128i color2;
						__m128i color3;
						__m128i color4;

#define sse_magic \
							color1 = _mm_cvtsi32_si128( samples[4] ); color1 = _mm_shuffle_epi32( color1, _MM_SHUFFLE( 0, 0, 0, 0 ) ); \
							color2 = _mm_cvtsi32_si128( samples[3] ); color2 = _mm_insert_epi32( color2, samples[5], 2 ); color2 = _mm_shuffle_epi32( color2, _MM_SHUFFLE( 2, 2, 0, 0 ) ); \
							color3 = _mm_cvtsi32_si128( samples[7] ); color3 = _mm_insert_epi32( color3, samples[1], 1 ); color3 = _mm_shuffle_epi32( color3, _MM_SHUFFLE( 0, 1, 1, 0 ) ); \
							color4 = _mm_cvtsi32_si128( samples[6] ); color4 = _mm_insert_epi32( color4, samples[0], 1 ); color4 = _mm_insert_epi32( color4, samples[2], 2 ); color4 = _mm_insert_epi32( color4, samples[8], 3 ); \
							color1 = _mm_adds_epu8( color1, color2 ); \
							color1 = _mm_adds_epu8( color1, color3 ); \
							color1 = _mm_adds_epu8( color1, color4 ); 
							//color1 = _mm_set_epi32( 0x3c000000, 0x3c000000, 0x3c000000, 0x3c000000 );

						int samples[9];

						if( mask&1 ) {
							int ixc2 = ixc + STEPCZ;
							samples[0] = bg_light_buffer[ixc2-1-STEPCY]; samples[1] = bg_light_buffer[ixc2  -STEPCY]; samples[2] = bg_light_buffer[ixc2+1-STEPCY];
							samples[3] = bg_light_buffer[ixc2-1       ]; samples[4] = bg_light_buffer[ixc2         ]; samples[5] = bg_light_buffer[ixc2+1       ];
							samples[6] = bg_light_buffer[ixc2-1+STEPCY]; samples[7] = bg_light_buffer[ixc2  +STEPCY]; samples[8] = bg_light_buffer[ixc2+1+STEPCY];
							sse_magic;
							copy_cube_face_i( s, *target, x, y, z, 0, color1 );
							(*target)++;
						}

						if( mask&2 ) { // back
							int ixc2 = ixc - STEPCZ;
							samples[0] = bg_light_buffer[ixc2+1-STEPCY]; samples[1] = bg_light_buffer[ixc2  -STEPCY]; samples[2] = bg_light_buffer[ixc2-1-STEPCY];
							samples[3] = bg_light_buffer[ixc2+1       ]; samples[4] = bg_light_buffer[ixc2         ]; samples[5] = bg_light_buffer[ixc2-1       ];
							samples[6] = bg_light_buffer[ixc2+1+STEPCY]; samples[7] = bg_light_buffer[ixc2  +STEPCY]; samples[8] = bg_light_buffer[ixc2-1+STEPCY];
							sse_magic;
							copy_cube_face_i( s, *target, x, y, z, 1, color1 );
							(*target)++;
						}

						if( mask&4 ) { //right
							int ixc2 = ixc + 1;
							samples[0] = bg_light_buffer[ixc2+STEPCZ-STEPCY]; samples[1] = bg_light_buffer[ixc2  -STEPCY]; samples[2] = bg_light_buffer[ixc2-STEPCZ-STEPCY];
							samples[3] = bg_light_buffer[ixc2+STEPCZ       ]; samples[4] = bg_light_buffer[ixc2         ]; samples[5] = bg_light_buffer[ixc2-STEPCZ       ];
							samples[6] = bg_light_buffer[ixc2+STEPCZ+STEPCY]; samples[7] = bg_light_buffer[ixc2  +STEPCY]; samples[8] = bg_light_buffer[ixc2-STEPCZ+STEPCY];
							sse_magic;
							copy_cube_face_i( s, *target, x, y, z, 2, color1 );
							(*target)++;
						}

						if( mask&8 ) { // left
							int ixc2 = ixc - 1;
							samples[0] = bg_light_buffer[ixc2-STEPCZ-STEPCY]; samples[1] = bg_light_buffer[ixc2  -STEPCY]; samples[2] = bg_light_buffer[ixc2+STEPCZ-STEPCY];
							samples[3] = bg_light_buffer[ixc2-STEPCZ       ]; samples[4] = bg_light_buffer[ixc2         ]; samples[5] = bg_light_buffer[ixc2+STEPCZ       ];
							samples[6] = bg_light_buffer[ixc2-STEPCZ+STEPCY]; samples[7] = bg_light_buffer[ixc2  +STEPCY]; samples[8] = bg_light_buffer[ixc2+STEPCZ+STEPCY];
							sse_magic;
							copy_cube_face_i( s, *target, x, y, z, 3, color1 );
							(*target)++;
						}

						if( mask&16 ) { // top
							int ixc2 = ixc + STEPCY;
							samples[0] = bg_light_buffer[ixc2-1+STEPCZ]; samples[1] = bg_light_buffer[ixc2  +STEPCZ]; samples[2] = bg_light_buffer[ixc2+1+STEPCZ];
							samples[3] = bg_light_buffer[ixc2-1       ]; samples[4] = bg_light_buffer[ixc2         ]; samples[5] = bg_light_buffer[ixc2+1       ];
							samples[6] = bg_light_buffer[ixc2-1-STEPCZ]; samples[7] = bg_light_buffer[ixc2  -STEPCZ]; samples[8] = bg_light_buffer[ixc2+1-STEPCZ];
							//samples[1]=samples[2]=samples[3]=samples[4]=samples[5]=samples[6]=samples[7]=samples[8]=samples[0]; halp


							sse_magic;
							copy_cube_face_i( s, *target, x, y, z, 4, color1 );
							(*target)++;
						}

						if( mask&32 ) { // bottom
							int ixc2 = ixc - STEPCY;
							samples[0] = bg_light_buffer[ixc2-1-STEPCZ]; samples[1] = bg_light_buffer[ixc2  -STEPCZ]; samples[2] = bg_light_buffer[ixc2+1-STEPCZ];
							samples[3] = bg_light_buffer[ixc2-1       ]; samples[4] = bg_light_buffer[ixc2         ]; samples[5] = bg_light_buffer[ixc2+1       ];
							samples[6] = bg_light_buffer[ixc2-1+STEPCZ]; samples[7] = bg_light_buffer[ixc2  +STEPCZ]; samples[8] = bg_light_buffer[ixc2+1+STEPCZ];
							sse_magic;
							copy_cube_face_i( s, *target, x, y, z, 5, color1 );
							(*target)++;
						}

					}

					

				} // air test

				assert( (((u32)write - (u32)vertexBuffer)) < sizeof(vertexBuffer) );
			}
		}
	}
	
	ri->vbosize = ((u32)write - (u32)vertexBuffer) / sizeof( Video::terrain_instance_vertex );
	ri->vbosize_water = ((u32)write_water - (u32)water_start) / sizeof( Video::terrain_instance_vertex );
	// move data

	u8 *cpy_source = (u8*)water_start;
	u8 *cpy_dest = (u8*)write;

	for( int i = 0; i < (int)(ri->vbosize_water * sizeof( Video::terrain_instance_vertex )); i++ ) {
		*cpy_dest++ = *cpy_source++;
	}
}

void computePlane( plane32 &dest, cml::vector3f &point1, cml::vector3f &point2, cml::vector3f &point3 ) {
	double x1,y1,z1; x1 = point1[0]; y1 = point1[1]; z1 = point1[2];
	double x2,y2,z2; x2 = point2[0]; y2 = point2[1]; z2 = point2[2];
	double x3,y3,z3; x3 = point3[0]; y3 = point3[1]; z3 = point3[2];

	dest.normal[0] = (float)(y1 * (z2 - z3) + y2 * (z3 - z1) + y3 * (z1 - z2));
	dest.normal[1] = (float)(z1 * (x2 - x3) + z2 * (x3 - x1) + z3 * (x1 - x2));
	dest.normal[2] = (float)(x1 * (y2 - y3) + x2 * (y3 - y1) + x3 * (y1 - y2));
	dest.d = (float)(x1 * (y2 * z3 - y3 * z2) + x2 * (y3 * z1 - y1 * z3) + x3 * (y1 * z2 - y2 * z1));
	dest.d = -dest.d;

	float w = cml::length(dest.normal);
	dest.normal /= w;
	dest.d /= w;
}

void computePlane2( plane2 &dest, cml::vector3f &point, cml::vector3f &a, cml::vector3f &b ) {
	dest.normal = cml::cross( a - point, b - point );
	dest.normal.normalize();
	dest.point = point;
}

float pointplanedist( plane2 &plane, cml::vector3f &point ) {
	cml::vector3f vec = point - plane.point;
	return cml::dot( plane.normal, vec );
}

float getdist2( renderingInstance *ri ) {
	cml::vector3f cam = Video::GetCamera();
	float vdx = cam[0]-(ri->x*32+16);
	float vdy = cam[1]-(ri->y*32+16);
	float vdz = cam[2]-(ri->z*32+16);
	float dist = vdx*vdx+vdy*vdy+vdz*vdz;
	return dist;
}

void sortInstances() {

	cml::vector3f cam = Video::GetCamera();
	 
	rl_free.erase();
	rl_active.erase();
	rl_resv.erase();
 
	// compute viewbox planes
	/*
	plane32 vpRight;
	plane32 vpLeft;
	plane32 vpTop;
	plane32 vpBottom;
	plane32 vpFront;
	plane32 vpBack;*/

	plane2 pfront, pback, pleft, pright, ptop, pbottom;

	cml::vector3f farplane[4];
	for( int i = 0; i <= 3; i++ )
		farplane[i] = cml::lerp( Video::nearPlane[i], Video::farPlane[i], Video::farPlaneZ() / 10000.0f );

	computePlane2( pback, farplane[3], farplane[0], farplane[2] );
	computePlane2( pfront, Video::nearPlane[2], Video::nearPlane[1], Video::nearPlane[3] );
	
	computePlane2( pright, Video::nearPlane[3], farplane[3], Video::nearPlane[2] );
	computePlane2( pleft, Video::nearPlane[0], Video::nearPlane[1], farplane[0] );

	computePlane2( ptop, Video::nearPlane[3], Video::nearPlane[0], farplane[3] );
	computePlane2( pbottom, Video::nearPlane[2], farplane[2], Video::nearPlane[1] );



	cml::vector3f dist;
	/*
	// right
	computePlane( vpRight, Video::nearPlane[3], Video::nearPlane[2], Video::farPlane[3] );
	computePlane( vpLeft, Video::nearPlane[1], Video::nearPlane[0], Video::farPlane[0] );
	computePlane( vpTop, Video::nearPlane[0], Video::nearPlane[3], Video::farPlane[3] );
	computePlane( vpBottom, Video::nearPlane[2], Video::nearPlane[1], Video::farPlane[1] );
	computePlane( vpFront, Video::nearPlane[3], Video::nearPlane[0], Video::nearPlane[1] );
	computePlane( vpBack, Video::farPlane[0], Video::farPlane[3], Video::farPlane[2] );
	*/
	double fd1,fd2,fd3,fd4,fd5,fd6;

	for( int i = 0; i < instanceCount; i++ ) {
		renderingInstance *inst = instances+i;

		double x = inst->x*32+16,y=inst->y*32+16,z=inst->z*32+16;
		cml::vector3f point((float)x,(float)y,(float)z);
	
		fd1 = pointplanedist( pfront, point );
		fd2 = pointplanedist( pback, point );
		fd3 = pointplanedist( pleft, point );
		fd4 = pointplanedist( pright, point );
		fd5 = pointplanedist( ptop, point );
		fd6 = pointplanedist( pbottom, point );

		/*
		fd1 = vpFront.normal[0] * x + vpFront.normal[1] * y + vpFront.normal[2] * z + vpFront.d;
		fd2 = vpBack.normal[0] * x + vpBack.normal[1] * y + vpBack.normal[2] * z + vpBack.d;
		fd3 = vpLeft.normal[0] * x + vpLeft.normal[1] * y + vpLeft.normal[2] * z + vpLeft.d;
		fd4 = vpRight.normal[0] * x + vpRight.normal[1] * y + vpRight.normal[2] * z + vpRight.d;
		fd5 = vpTop.normal[0] * x + vpTop.normal[1] * y + vpTop.normal[2] * z + vpTop.d;
		fd6 = vpBottom.normal[0] * x + vpBottom.normal[1] * y + vpBottom.normal[2] * z + vpBottom.d;
		*/

		double vdx = cam[0]-x;
		double vdy = cam[1]-y;
		double vdz = cam[2]-z;
		double dist = vdx*vdx+vdy*vdy+vdz*vdz;


		bool active = dist<1600 ||
				(fd1 > -28) && (fd2 > -28) &&
				(fd3 > -28) && (fd4 > -28) &&
				(fd5 > -28) && (fd6 > -28);
	
		if( (inst->map_index<0) || !inst->registered ) {
			active=false;
		//} else if( !planet::real_chunk( *inst->data_ref) ) {
		//	active=false;
		}


		if( active ) {
			rl_resv.add(inst);

			//if( inst->data_ref ) {
			//	if( planet::real_chunk(*inst->data_ref) ) {
					//planet::refresh_chunk(*inst->data_ref);
			//	}
			//}

		} else {
			rl_free.add(inst);
			
		}
	}
	

	while(!rl_free.empty()) {
		renderingInstance *ri = rl_free.remove(rl_free.get_first());
		float f = getdist2( ri );
		if( f < 200*200  ){
			rl_1.add(ri);
		} else if( f < 300*300 ) {
			rl_2.add(ri);
		} else if( f < 400*400 ) {
			rl_3.add(ri);
		} else {
			rl_4.add(ri);
		}
	}
	
	while(!rl_4.empty()) rl_free.add(rl_4.remove(rl_4.get_first()));
	while(!rl_3.empty()) rl_free.add(rl_3.remove(rl_3.get_first()));
	while(!rl_2.empty()) rl_free.add(rl_2.remove(rl_2.get_first()));
	while(!rl_1.empty()) rl_free.add(rl_1.remove(rl_1.get_first()));
}

// todo: what is this
/*
void reset_instance( int index ) {
	renderingInstance *ri = instances+index;
	ri->vbosize=0;
	//ri->data_ref=0;
	ri->planet_index = -1;
}*/
/*
void light_spread_r( lightInstance *light, int x, int y, int z, int intensity ) {
	int cx, cy, cz;
	cx = (light->x-16+x);
	cy = (light->y-16+y);
	cz = (light->z-16+z);
	
	if( intensity > light->stamp[x+y*1024+z*32] ) {
		
		planet::chunk *ch = planet::get_chunk(planet::get_chunk_pointer(cx>>4,cy>>4,cz>>4));
		if( !ch ) return;
		intensity -= planet::get_opacity( ch, cx&15, cy&15, cz&15 );
		if( intensity > light->stamp[x+y*1024+z*32] ) {
			light->stamp[x+y*1024+z*32] = intensity;
			if( intensity > 1 )  {
				light_spread_r( light, x-1, y, z, intensity-1 );
				light_spread_r( light, x+1, y, z, intensity-1 );
				light_spread_r( light, x, y-1, z, intensity-1 );
				light_spread_r( light, x, y+1, z, intensity-1 );
				light_spread_r( light, x, y, z-1, intensity-1 );
				light_spread_r( light, x, y, z+1, intensity-1 );
			}
		}
	} else {
		return;
	}
}

void light_spread( lightInstance *light ) {
	light->dirty=0;
	for( int i = 0; i < 32*32*32; i++ ) {
		light->stamp[i] = 0;
	}
	//light->stamp[8+8*256+8*16] = light->intensity;

	light_spread_r( light, 16, 16, 16, light->intensity );
}*/

lightInstance2 * light_create( float brightness, float r, float g, float b ) {
	lightInstance2 *light = lights.add();
	light->brightness = brightness;
	light->r = r;
	light->g = g;
	light->b = b;

	update_light_uniforms = true;
	return light;
}

void light_destroy( lightInstance2 *light ) {

	lights.remove( light );

	update_light_uniforms = true;
}

void light_setpos( lightInstance2 *light, float x, float y, float z ) {
	light->x = x;
	light->y = y;
	light->z = z;
	update_light_uniforms=true;
	
}

void lights_update() {
	/*
	for( lightInstance *light = lights.get_first(); light; light = light->next ) {
		light->dirty=1; // todo: is this a debug line?

		if( light->px != light->x || light->py != light->y || light->pz != light->z ) {
			
			for( int ix = 0; ix <= 2; ix++ ) {
			for( int iy = 0; iy <= 2; iy++ ) {
			for( int iz = 0; iz <= 2; iz++ ) {
				planet::chunk *c = planet::get_chunk(planet::get_chunk_pointer( (light->px-16+ix*15)>>4, (light->py-16+iy*15)>>4, (light->pz-16+iz*15)>>4 ));
				if( c ) {
					if( c->instance != 0 ) {
						renderingInstance *ri = get_instance(c->instance-1);
						ri->dirty=1;
						ri->dimlight_changed=1;
					}
					//if( c->dirty ) {
					//	light->dirty=1;
					//}
				}
			}}}
			light->px = light->x;
			light->py = light->y;
			light->pz = light->z;
		}

		if( !light->dirty ) {
			for( int ix = 0; ix <= 2; ix++ ) {
			for( int iy = 0; iy <= 2; iy++ ) {
			for( int iz = 0; iz <= 2; iz++ ) {
				planet::chunk *c = planet::get_chunk(planet::get_chunk_pointer( (light->x-16+ix*15)>>4, (light->y-16+iy*15)>>4, (light->z-16+iz*15)>>4 ));
				if( c ) {
					if( c->instance ) {
						renderingInstance *ri = get_instance(c->instance-1);
						if( ri->dirty ) light->dirty = 1;
					}
					
				}
			}}}
		} else {
			for( int ix = 0; ix <= 2; ix++ ) {
			for( int iy = 0; iy <= 2; iy++ ) {
			for( int iz = 0; iz <= 2; iz++ ) {
				planet::chunk *c = planet::get_chunk(planet::get_chunk_pointer( (light->x-16+ix*15)>>4, (light->y-16+iy*15)>>4, (light->z-16+iz*15)>>4 ));
				if( c ) {
					if( c->instance ) {
						renderingInstance *ri = get_instance(c->instance-1);
						ri->dirty=1;
						ri->dimlight_changed=1;
					}
					//c->dirty=1;
					//c->dimlight_changed=1;
					
				}
			}}}
		}
		
		if( light->dirty ) {
			light_spread( light );
		}
	}
	*/
}

void dirty_instance( int index, bool geometry, bool dimlight, bool skylight ) {
	renderingInstance *ri;
	ri = get_instance(index);
	if( geometry ) ri->dirty = true;
	if( dimlight ) ri->dimlight_changed = true;
	if( skylight ) ri->skylight_changed = true;
}

bool clamped_solid_test( float vx, float vy, float vz, int lcx, int lcy, int lcz ) {
	int cx = (int)floor(vx);
	int cy = (int)floor(vy);
	int cz = (int)floor(vz);

	cx -= lcx<<4;
	cy -= lcy<<4;
	cz -= lcz<<4;
	if( cx < 0 ) cx = 0;
	if( cy < 0 ) cy = 0;
	if( cz < 0 ) cz = 0;
	if( cx > 15 ) cx = 15;
	if( cy > 15 ) cy = 15;
	if( cz > 15 ) cz = 15;
	cx += lcx<<4;
	cy += lcy<<4;
	cz += lcz<<4;
	return planet::collision( cx, cy, cz );
}

void RegisterInstance( renderingInstance *ri ) {
	
	set_instance_map( ri->map_index, ri->index+1 );
	skylight_manager.Register( ri->x, ri->z );
	ri->registered = true;
}

void UnregisterInstance( renderingInstance *ri ) {
	if( ri->registered ) {
		skylight_manager.Unregister( ri->x, ri->z );
		
		int y_s, z_s;
		z_s = planet::get_width() / 16;
		y_s = z_s * planet::get_length() / 16;
		reset_instance_map( ri->map_index );
		//for( int x = 0; x < 2; x++ ) { for( int y = 0; y < 2; y++ ) { for( int z = 0; z < 2; z++ ) {
		//	CHUNKPTR p = planet::get_chunk_pointer( ri->planet_index + x + y * y_s + z * z_s );
		//	if( planet::real_chunk(p) ) {
		//		planet::get_chunk(p)->instance = 0;
		//	}
		//}}}

		/*
		if( ri->data_ref ) {
			if( planet::real_chunk(*ri->data_ref) ) {
				planet::get_chunk((*ri->data_ref))->instance = 0;
			}
		}*/
		ri->registered = false;
	}
}



void dirty_pancake( int x, int y, int z, bool geometry, bool dimlight, bool skylight ) {
	for( int cz = -1; cz <= 1; cz++ ) {
		for( int cx = -1; cx <= 1; cx++ ) {

			int index = read_instance_map_from_planet_index( (x+cx) ,y ,(z+cz)  );
			if(index > 0 ){
				dirty_instance( index-1, geometry, dimlight, skylight );
			}
			
			//CHUNKPTR p = planet::get_chunk_pointer( x+cx,y,z+cz );
			//if( planet::real_chunk(p) ) {
			//	planet::chunk *c = planet::get_chunk(p);
			//	
			//	if( c->instance != 0 ){
			//		dirty_instance(c->instance-1,geometry, dimlight, skylight );
			//		/*
			//		renderingInstance &inst = instances[c->instance-1];
			//		inst.dirty = inst.dirty || geometry;
			//		inst.dimlight_changed = inst.dimlight_changed || dimlight;
			//		inst.skylight_changed = inst.skylight_changed || skylight;
			//		*/
			//	}
			//}
		}
	}
}

void dirty_area( int x, int y, int z, bool geometry, bool dimlight, bool skylight ) {
	dirty_pancake(x,y-1,z,geometry,dimlight,skylight);
	dirty_pancake(x,y,z,geometry,dimlight,skylight);
	dirty_pancake(x,y+1,z,geometry,dimlight,skylight);
	/*
	for( int cy = -1; cy <= 1; cy++ ) {
		for( int cz = -1; cz <= 1; cz++ ) {
			for( int cx = -1; cx <= 1; cx++ ) {
				CHUNKPTR p = planet::get_chunk_pointer( x+cx,y+cy,z+cz );
				if( planet::real_chunk(p) ) {
					planet::chunk *c = planet::get_chunk(p);
					
					if( c->instance != 0 ){
						renderingInstance &inst = instances[c->instance-1];
						inst.dirty = inst.dirty || geometry;
						inst.dimlight_changed = inst.dimlight_changed || dimlight;
						inst.skylight_changed = inst.skylight_changed || skylight;
					}
				}
			}
		}
	}
	*/
}

void update() { 
	sortInstances();

	cml::vector3f cam = Video::GetCamera();
	cml::vector3d plane_n[4];
	cml::vector3d plane_f[4];

	// copy near/far planes
	for( int i = 0; i < 4; i++ ) {
		plane_n[i] = Video::nearPlane[i];
		plane_f[i] = Video::farPlane[i];
	}

	plane_n[1] = plane_n[1] - plane_n[0];
	plane_n[3] = plane_n[3] - plane_n[0];
	plane_f[1] = plane_f[1] - plane_f[0];
	plane_f[3] = plane_f[3] - plane_f[0];

	int load_threshold=5; // x chunk updates/frame max (?)
	
	
	// cast testing rays
	debug_count_2=0; 

	float dither_x = 0;
	float dither_y = 0;

	float xstep = 1.0f/(1920.0f/32.0f);
	float ystep = 1.0f/(1080.0f/32.0f);

	xstep = 0.05f;
	ystep = 0.05f;

	dither_x = ((float)rand() / (float)RAND_MAX) * xstep;
	dither_y = ((float)rand() / (float)RAND_MAX) * ystep;
	
	for( float x = -0.0f + dither_x; x <= 1.0f; x += xstep ) {
						 
		for( float y = -0.0f + dither_y; y <= 1.0f; y += ystep ) {
				
			// create ray

			cml::vector3f vec = plane_n[0] + plane_n[3]*x + plane_n[1]*y;
			cml::vector3f vfar = plane_f[0] + plane_f[3]*x + plane_f[1]*y;
				
			cml::vector3f vlength = (vfar - vec);// / Video::farPlaneZ() * 16;
			cml::vector3f inc = vlength;
				
			inc.normalize();
				
			float total_length_max = Video::farPlaneZ()-32;
				
			int lcx, lcy, lcz;
				
			lcx = -1;
			lcy = -1;
			lcz = -1;
				
			float nx_a,ny_a,nz_a;
			nx_a = inc[0] > 0.0f ? 16.0f : 0.0f;
			ny_a = inc[1] > 0.0f ? 16.0f : 0.0f;
			nz_a = inc[2] > 0.0f ? 16.0f : 0.0f;
			float incr[3];
			incr[0] = 1.0f/inc[0];
			incr[1] = 1.0f/inc[1];
			incr[2] = 1.0f/inc[2];
				
			for( float total_length=0; total_length < total_length_max ; ) {
			//for( i_total_length=0; i_total_length < i_total_length_max ; ) {
				int cx, cy, cz;
				cx = (int)floor(vec[0]);
				cy = (int)floor(vec[1]);
				cz = (int)floor(vec[2]);

				float nx,ny,nz;
				nx = (float)floor(vec[0]/16.0f)*16.0f;
				ny = (float)floor(vec[1]/16.0f)*16.0f;
				nz = (float)floor(vec[2]/16.0f)*16.0f;
				nx+=nx_a;
				ny+=ny_a;
				nz+=nz_a;


				nx -= vec[0];
				ny -= vec[1];
				nz -= vec[2];
				nx *= incr[0];
				ny *= incr[1];
				nz *= incr[2];


				int ndir = 0; // X
				int solid=0;
					 
				if( ny < nx ) {
					nx = ny;
					ndir=1; // Y
				} 
				if( nz < nx ) {
					nx = nz;
					ndir = 2; // Z
				}
					 
				nx += 1.0;

				vec += inc*nx;
				total_length += nx;
					 
				if( cx >= planet::get_width() || (cz >= planet::get_length()) ) continue;
					 

				lcx = cx>>4;
				lcy = cy>>4;
				lcz = cz>>4;

				if( !(lcy >= 0 && (lcy<<4) < planet::get_depth() ) ) {
					// end ray (player should not be out of bounds)
						 
					break;

				}

				CHUNKPTR cs = planet::get_chunk_pointer( lcx, lcy, lcz );
				int state = planet::read_state(cs);

				if( !planet::real_chunk(cs) ) {
						
					if( state == planet::CHUNK_INVALID ) {
						planet::request( lcx,lcy,lcz );
							 
						break;
							 
					} else if( state == planet::CHUNK_EMPTY ) {
							 
					} else {
						break;
					}
				}

				if( state != planet::CHUNK_EMPTY ) {

					planet::chunk *ch = planet::get_chunk(cs);
					if( read_instance_map_from_planet_index( lcx, lcy, lcz ) > 0 ) {

						int pooda = 5;

						
					//if( ch->instance ) {
						// chunk already referenced
						// todo: is this code needed? it should already be in the active list after the sort
						//       (this code might conflict with instances on the far viewing edge)

						/*
						renderingInstance *ri = get_instance(ch->instance-1);
							
						if( ri->parent != &rl_active ) {
							ri->parent->remove(ri);
							rl_active.add(ri);
								
						}
						*/

						
					} else {
						if( !rl_free.empty() ) {
							renderingInstance *ri;
								
							ri = rl_free.remove(rl_free.get_first());
								
							rl_active.add(ri);

							UnregisterInstance(ri);
							/*if( ri->data_ref ) {
								if( planet::real_chunk(*ri->data_ref) ) {
									planet::get_chunk((*ri->data_ref))->instance = 0;
								}
							} (moved) */
							

							//ch->instance = 1+ri->index;

							//ri->data_ref = planet::get_chunk_slot(lcx,lcy,lcz);
							ri->x = lcx>>1;
							ri->y = lcy>>1;
							ri->z = lcz>>1;
							ri->map_index = to_instance_map_index( ri->x, ri->y, ri->z );


							ri->dirty=1;
							ri->gdelay=10; // delay X frames before geometry
							ri->skylight_changed=1;
							ri->dimlight_changed=1;
							ri->vbosize =0;
							ri->vbosize_water =0;
							ri->opacity = 0;

							RegisterInstance( ri );
								
							//break;
						} else {

							
							// error: no more instances
							//printf(" rand out of instances\n");
							//if( b->original ) {
								/// 
							//}
	//						ray_stoppers[ray_index]=1;
							break;
						}

					}

					solid = clamped_solid_test( vec[0], vec[1], vec[2], lcx, lcy, lcz );
					solid = solid || planet::collision( cx,cy,cz);
					 
					if( solid ){
		//				ray_stoppers[ray_index]=1;
						break;
					}

				}
				
			}
		}
	}
	//}
	
	while(!rl_active.empty()) {
		renderingInstance *ri = rl_active.remove(rl_active.get_first());
		float f = getdist2( ri );
		if( f < 200*200  ){
			rl_1.add(ri);
		} else if( f < 300*300 ) {
			rl_2.add(ri);
		} else if( f < 400*400 ) {
			rl_3.add(ri);
		} else {
			rl_4.add(ri);
		}
	}
	
	while(!rl_resv.empty()) {
		//rl_active.add(rl_resv.remove(rl_resv.get_first()));

		renderingInstance *ri = rl_resv.remove(rl_resv.get_first());
		float f = getdist2( ri );
		if( f < 200*200 ){
			rl_1.add(ri);
		} else if( f < 300*300 ) {
			rl_2.add(ri);
		} else if( f < 400*400 ) {
			rl_3.add(ri);
		} else {
			rl_4.add(ri);
		}
	}

	while(!rl_1.empty()) rl_active.add(rl_1.remove(rl_1.get_first()));
	while(!rl_2.empty()) rl_active.add(rl_2.remove(rl_2.get_first()));
	while(!rl_3.empty()) rl_active.add(rl_3.remove(rl_3.get_first()));
	while(!rl_4.empty()) rl_active.add(rl_4.remove(rl_4.get_first()));

	//planet::do_jobs();

	lights_update();

	planet::do_render_jobs();

	for( renderingInstance *ri = rl_active.get_first(); ri; ri = ri->next ) {
		if( ri->registered ) {
		//if(ri->data_ref){
			//if( planet::real_chunk(*ri->data_ref) ) {
				//planet::chunk *ch = planet::get_chunk(*ri->data_ref);
//				if( (*ri->data_ref)->network_request ) continue;
				//if( skylight_manager.IsDirty( ri->x, ri->z ) ) {
					skylight_manager.Update( ri->x, ri->z );
				//}
				/*
				if( planet::is_sunlight_dirty(ri->x,ri->z) ) {
					planet::recalc_sunlight( ri->x, ri->z );
					//if( (*ri->data_ref)->sunlight_dirty ) {
					
					//debug_instance_count++;
//					planet::sunlight_beam(ri->x,ri->y,ri->z);
					//planet::recalc_sunlight_chunk(ri->x,ri->y,ri->z);
					
				}*/

				//if( (*ri->data_ref)->sunlight_spread ) {
				//	(*ri->data_ref)->sunlight_spread = 0;
					//planet::spread_sunlight(ri->x,ri->y,ri->z);
				//}

				// TODO: !!
//				if( ch->modified ) {	TODO: FIX THIS
//					planet::rebuild_solid_bitmap(*ri->data_ref);
//					ch->modified=0;
//				}

			
		}
	}

	//debug_instance_count=0;
	/*
	for( renderingInstance *ri = rl_active.get_first(); ri; ri = ri->next ) {
		if(ri->data_ref){
			if( valid_chunk_addr(*ri->data_ref) ) {
				//if( (*ri->data_ref)->network_request ) continue;
				if( (*ri->data_ref)->dimlight_changed ) {
					(*ri->data_ref)->dimlight_changed=0;
					//debug_instance_count++;
					//planet::spread_light(ri->x,ri->y,ri->z);
				}
			}
		}
	}*/

					//

	
	//profStart();

	debug_instance_count=0;
	for( renderingInstance *ri = rl_active.get_first(); ri; ri = ri->next ) {
		if( ri->opacity < 1.0f ) {
			if( ri->gdelay == 0 )
				ri->opacity += 0.05f;
		}
		debug_instance_count++;
		if( ri->registered ) {
		//if( ri->data_ref ) {
			//if( planet::real_chunk(*ri->data_ref) ) {
				//if( (*ri->data_ref)->network_request ) {
				//	if( (*ri->data_ref)->network_request == planet::NETWORK_REQUEST_NEEDED ) {
				//		(*ri->data_ref)->network_request = planet::NETWORK_REQUEST_ISSUED;
				//		
				//		network::csWorldChunkRequest( planet::getWorldIndex( ri->x, ri->y, ri->z ) );
				//	}
				//	continue;
				//}
				if( ri->dirty || ri->skylight_changed || ri->dimlight_changed ) {//||(*ri->data_ref)->dirty ) {

					if( ri->gdelay ) {
						ri->gdelay--;
					} else {
						ri->dirty=0;
						//(*ri->data_ref)->dirty=0;
			//			planet::chunk *left,*right,*front,*back,*top,*bottom;
				//		left = planet::get_chunk_direct((ri->x-1) & planet::PLANET_MASK,ri->y,ri->z);
			//			right=planet::get_chunk_direct((ri->x+1) & planet::PLANET_MASK,ri->y,ri->z);
				//		front=planet::get_chunk_direct(ri->x,ri->y,(ri->z+1) & planet::PLANET_MASK);
			//			back=planet::get_chunk_direct(ri->x,ri->y,(ri->z-1) & planet::PLANET_MASK);
				//		top = ((ri->y+1) < planet::PLANET_HEIGHT) ? planet::get_chunk_direct(ri->x,ri->y+1,ri->z) : 0;
			//			bottom = ((ri->y-1) > 0) ? planet::get_chunk_direct(ri->x,ri->y-1,ri->z) : 0;


						buildGeometry2( ri );// (*ri->data_ref), left,right,front,back,top,bottom,ri->vbosize,ri->vbosize_water );
							
						vertex_buffer.BufferData( vertexBuffer, (ri->vbosize+ri->vbosize_water) * sizeof(Video::terrain_instance_vertex), GL_STATIC_DRAW_ARB, ri->index );
						//glBindBufferARB( GL_ARRAY_BUFFER_ARB, vboIDs[ri->index] );
						//glBufferDataARB( GL_ARRAY_BUFFER_ARB, ri->vbosize, vertexBuffer, GL_STATIC_DRAW_ARB );

						//glBindBufferARB( GL_ARRAY_BUFFER_ARB, vboIDs[ri->index+instanceCount] );
						//glBufferDataARB( GL_ARRAY_BUFFER_ARB, ri->vbosize_water, waterBuffer, GL_STATIC_DRAW_ARB );

						load_threshold--; 
						if( load_threshold <= 0 ) break;
						//debug_instance_count++;
					}
				}

//				if( (*ri->data_ref)->fade < 64 ) (*ri->data_ref)->fade++;
			//}
		}
	}
	
	//profEnd(1);
	
	//profEnd(1);
}

void terrain_particle( int px, int py, int pz, int block ) {

	float x = (float)px;
	float y = (float)py;
	float z = (float)pz;
 
	int k;
	int count=1;
		

	switch( block ){
	case planet::CELL_DIRT:
		k = particles::PK_DIRT_PARTICLES;
		
		break;
	case planet::CELL_GRASS:
	case planet::CELL_FORESTGROUND:
		
		k = particles::PK_GRASS_PARTICLES;
		
		break;
	case planet::CELL_LEAVES:
		k = particles::PK_GRASS_PARTICLES;
		
		break;
	case planet::CELL_TREE:
		k = particles::PK_DIRT_PARTICLES;
		break;
	case planet::CELL_ROCK:
	case planet::CELL_SAND:
	default:
		return;
	} 
	
	for( ;count;count-- )
		particles::add( k, cml::vector3f( x,y,z ) );

}

float sun_r,sun_g,sun_b, sun_i,sun_a;
float fog_r, fog_g, fog_b;
void set_sunlight_color( float r, float g, float b ) {
	sun_r=r;
	sun_g=g;
	sun_b=b;
}

void set_fog_color( float r, float g, float b ) {
	fog_r = r;
	fog_g = g;
	fog_b = b;
}

void set_sunlight_intensity( float i ) {
	sun_i = i;
}

void apply_sunlight( int &r, int &g, int &b, int sun ) {
	float s = (float)sun ;
	r = r + (int)((sun_r * s * sun_i) * 128);
	g = g + (int)((sun_g * s * sun_i) * 128);
	b = b + (int)((sun_b * s * sun_i) * 128);
}

//void setup_shader_sun( int tint, int intensity ) {
	
//	glUniform3f( tint, sun_r, sun_g, sun_b );
//	glUniform1f( intensity, sun_i );
//}
//
//void set_sunlight_position( float angle ) {
//	sun_a = angle;
//}

void render() {
	//return;// debug bypass
	Video::SetShader( Video::SHADER_TERRAIN );
	
	Video::SetBlendMode( Video::BLEND_OPAQUE );
	
	Video::SetTerrainShaderNoise( 2 );
	Video::SetTerrainShaderPatterns( 1 );
	Video::SetTerrainShaderTexturing( 0 );
	Video::SetTerrainShaderFog( Video::farPlaneZ(), Video::get_fog_length() );
	Video::SetTerrainShaderFogColor( fog_r, fog_g, fog_b );
	Video::SetTerrainShaderSkylight( sun_r, sun_g, sun_b, sun_i );
	Video::SetTerrainShaderTextureTranslation( 0.0f, 0.0f, 0.0f );
	Video::UpdateTerrainShaderCamera();

	if( update_light_uniforms ) {
		update_light_uniforms=false;
		int total = 0;

		float positions[32*3];
		float colors[32*3];
		float brightness[32];

		float *wp = positions;
		float *wc = colors;
		float *wb = brightness;

		for( lightInstance2 *l = lights.get_first(); l; l = l->next ) {
			*wp++ = l->x;
			*wp++ = l->y;
			*wp++ = l->z;
			*wc++ = l->r;
			*wc++ = l->g;
			*wc++ = l->b;
			*wb++ = l->brightness;
			
			total++;
			if( total == 32 ) break;
		}

		for( ; total < 32; total++ ) {
			*wp++ = 0.0f;
			*wp++ = 0.0f;
			*wp++ = 0.0f;
			*wc++ = 0.0f;
			*wc++ = 0.0f;
			*wc++ = 0.0f;
			*wb++ = 0.0f;
		}

		Video::SetTerrainShaderLights( positions, colors, brightness );
	}
	
	/*
	Video::SetTerrainShaderSamplers( 0, 1 );
	Video::SetTerrainShaderFog( Video::farPlaneZ(), Video::get_fog_length() );
	
	Video::SetTerrainShaderSunlight( sun_r, sun_g, sun_b, sun_i );
	Video::SetTerrainShaderFogColor( fog_r, fog_g, fog_b );
	
	Video::SetTerrainShaderTranslation( 0.0f, 0.0f, 0.0f );
	Video::UpdateTerrainShaderCamera();
	
	Video::SetTerrainShaderTextureTranslation( 0, 0 );*/
	
	Video::SetActiveTextureSlot(2);
	Textures::Bind(Textures::NOISE);
	Video::SetActiveTextureSlot(1);
	Textures::Bind(Textures::TERRAIN_PATTERNS);
	Video::SetActiveTextureSlot(0);
	Textures::BindArray(Textures::WORLD);
	
	glCullFace(GL_BACK);
	Video::SetBlendMode( Video::BLEND_ALPHA );
	
	// iterate through and render active instances
	for( renderingInstance *ri = rl_active.get_first(); ri; ri=ri->next ) {
		//if( ri->data_ref ) {
		if( ri->registered ) {
			//if( (planet::real_chunk(*ri->data_ref)) && (ri->vbosize > 0) ) {
			if( ri->vbosize > 0 ) {
				//if( (*ri->data_ref)->network_request ) continue;
				float x = (float)ri->x * 32.0f;
				float y = (float)ri->y * 32.0f;
				float z = (float)ri->z * 32.0f;
				
				Video::SetTerrainShaderGlobalTranslation( x, y, z );
				Video::SetTerrainShaderOpacity( ri->opacity ); // todo
				
				
				vertex_buffer.Bind( ri->index );
				Video::SetShaderVertexAttributePointers(0,1);
				
				Video::DrawQuadsInstanced( 0, 4, ri->vbosize );
			}
		}
	}
	
	
}

float pooda; // todo: clean

void render_water() {
	//return; // debug bypass
	Video::SetShader( Video::SHADER_TERRAIN );
	
	Video::SetBlendMode( Video::BLEND_OPAQUE );
	
	Video::SetTerrainShaderNoise( 2 );
	Video::SetTerrainShaderPatterns( 1 );
	Video::SetTerrainShaderTexturing( 0 );
	Video::SetTerrainShaderFog( Video::farPlaneZ(), Video::get_fog_length() );
	Video::SetTerrainShaderFogColor( fog_r, fog_g, fog_b );
	Video::SetTerrainShaderSkylight( sun_r, sun_g, sun_b, sun_i );
	Video::UpdateTerrainShaderCamera();

	pooda += gametime::frames_passed() * 0.5f;
	Video::SetTerrainShaderTextureTranslation( 0, 0, fmod(floor(pooda) * 1.0f, 32.0f) );
	
	Video::SetActiveTextureSlot(2);
	Textures::Bind(Textures::NOISE);
	Video::SetActiveTextureSlot(1);
	Textures::Bind(Textures::TERRAIN_PATTERNS);
	Video::SetActiveTextureSlot(0);
	Textures::BindArray(Textures::WATER);	

	if( planet::get_cube( (int)(Video::GetCamera()[0]), (int)(Video::GetCamera()[1]), (int)(Video::GetCamera()[2]) ) == planet::CELL_WATER ) {
		glCullFace(GL_FRONT);
	} else {
		glCullFace(GL_BACK);
	}

	Video::SetBlendMode( Video::BLEND_ALPHA );

	for( renderingInstance *ri = rl_active.get_first(); ri; ri=ri->next ) {
		if( ri->registered ) {
			if( (ri->vbosize_water > 0) ) {
//				if( (*ri->data_ref)->network_request ) continue;
				//debug_instance_count++;
				float x = (float)ri->x * 32.0f;
				float y = (float)ri->y * 32.0f;
				float z = (float)ri->z * 32.0f;
				Video::SetTerrainShaderGlobalTranslation( x, y, z );
				Video::SetTerrainShaderOpacity( ri->opacity ); // todo
				
				vertex_buffer.Bind( ri->index );
				Video::SetShaderVertexAttributePointers( ri->vbosize * sizeof( Video::terrain_instance_vertex ), 1 );
				
				 Video::DrawQuadsInstanced( 0, 4, ri->vbosize_water );
				
			}
		}
	}
	glCullFace(GL_BACK);
}

enum {
	LEFT_BOTTOM_BACK,
	RIGHT_BOTTOM_BACK,
	LEFT_BOTTOM_FRONT,
	RIGHT_BOTTOM_FRONT,

	LEFT_TOP_BACK,
	RIGHT_TOP_BACK,
	LEFT_TOP_FRONT,
	RIGHT_TOP_FRONT

};

void build_simple_forms() {
	cml::vector3f cube_corners[8];
	cube_corners[LEFT_BOTTOM_BACK]		= cml::vector3f( 0.0f, 0.0f, 0.0f );
	cube_corners[RIGHT_BOTTOM_BACK]		= cml::vector3f( 1.0f, 0.0f, 0.0f );
	cube_corners[LEFT_BOTTOM_FRONT]		= cml::vector3f( 0.0f, 0.0f, 1.0f );
	cube_corners[RIGHT_BOTTOM_FRONT]	= cml::vector3f( 1.0f, 0.0f, 1.0f );
	cube_corners[LEFT_TOP_BACK]			= cml::vector3f( 0.0f, 1.0f, 0.0f );
	cube_corners[RIGHT_TOP_BACK]		= cml::vector3f( 1.0f, 1.0f, 0.0f );
	cube_corners[LEFT_TOP_FRONT]		= cml::vector3f( 0.0f, 1.0f, 1.0f );
	cube_corners[RIGHT_TOP_FRONT]		= cml::vector3f( 1.0f, 1.0f, 1.0f );

	cml::vector3f cube_normals;
	
	
	cml::vector3f form[4];
	form[0] = cube_corners[LEFT_TOP_FRONT];
	form[1] = cube_corners[LEFT_BOTTOM_FRONT];
	form[2] = cube_corners[RIGHT_BOTTOM_FRONT];
	form[3] = cube_corners[RIGHT_TOP_FRONT];
	set_form_pattern( FORM_CUBE_FRONT, form );

	form[0] = cube_corners[RIGHT_TOP_BACK];
	form[1] = cube_corners[RIGHT_BOTTOM_BACK];
	form[2] = cube_corners[LEFT_BOTTOM_BACK];
	form[3] = cube_corners[LEFT_TOP_BACK];
	set_form_pattern( FORM_CUBE_BACK, form );
	
	form[0] = cube_corners[RIGHT_TOP_FRONT];
	form[1] = cube_corners[RIGHT_BOTTOM_FRONT];
	form[2] = cube_corners[RIGHT_BOTTOM_BACK];
	form[3] = cube_corners[RIGHT_TOP_BACK];
	set_form_pattern( FORM_CUBE_RIGHT, form );

	form[0] = cube_corners[LEFT_TOP_BACK];
	form[1] = cube_corners[LEFT_BOTTOM_BACK];
	form[2] = cube_corners[LEFT_BOTTOM_FRONT];
	form[3] = cube_corners[LEFT_TOP_FRONT];
	set_form_pattern( FORM_CUBE_LEFT, form );

	form[0] = cube_corners[LEFT_TOP_BACK];
	form[1] = cube_corners[LEFT_TOP_FRONT];
	form[2] = cube_corners[RIGHT_TOP_FRONT];
	form[3] = cube_corners[RIGHT_TOP_BACK];
	set_form_pattern( FORM_CUBE_TOP, form );

	form[0] = cube_corners[LEFT_BOTTOM_FRONT];
	form[1] = cube_corners[LEFT_BOTTOM_BACK];
	form[2] = cube_corners[RIGHT_BOTTOM_BACK];
	form[3] = cube_corners[RIGHT_BOTTOM_FRONT];
	set_form_pattern( FORM_CUBE_BOTTOM, form );

	form[0] = cube_corners[LEFT_TOP_FRONT];
	form[1] = cube_corners[LEFT_BOTTOM_FRONT];
	form[2] = cube_corners[RIGHT_BOTTOM_BACK];
	form[3] = cube_corners[RIGHT_TOP_BACK];
	set_form_pattern( FORM_CROSS1, form );

	form[0] = cube_corners[LEFT_TOP_BACK];
	form[1] = cube_corners[LEFT_BOTTOM_BACK];
	form[2] = cube_corners[RIGHT_BOTTOM_FRONT];
	form[3] = cube_corners[RIGHT_TOP_FRONT];
	set_form_pattern( FORM_CROSS2, form );
}

void build_simple_normals() {
	cml::vector3f normal[4];
	cml::vector2f mt_coords;
	normal[0] = normal[1] = normal[2] = normal[3] = cml::vector3f( 0.0f, 0.0f, 1.0f );
	mt_coords.set( MT_SELX, MT_SELY );
	set_normal_pattern( FORM_CUBE_FRONT, normal );
	set_metatexture_coordinates_pattern( FORM_CUBE_FRONT, mt_coords );

	normal[0] = normal[1] = normal[2] = normal[3] = cml::vector3f( 0.0f, 0.0f, -1.0f );
	mt_coords.set( MT_SELX, MT_SELY );
	set_normal_pattern( FORM_CUBE_BACK, normal );
	set_metatexture_coordinates_pattern( FORM_CUBE_BACK, mt_coords );
	
	normal[0] = normal[1] = normal[2] = normal[3] = cml::vector3f( 1.0f, 0.0f, 0.0f );
	mt_coords.set( MT_SELZ, MT_SELY );
	set_normal_pattern( FORM_CUBE_RIGHT, normal );
	set_metatexture_coordinates_pattern( FORM_CUBE_RIGHT, mt_coords );

	normal[0] = normal[1] = normal[2] = normal[3] = cml::vector3f( -1.0f, 0.0f, 0.0f );
	mt_coords.set( MT_SELZ, MT_SELY );
	set_normal_pattern( FORM_CUBE_LEFT, normal );
	set_metatexture_coordinates_pattern( FORM_CUBE_LEFT, mt_coords );

	normal[0] = normal[1] = normal[2] = normal[3] = cml::vector3f( 0.0f, 1.0f, 0.0f );
	mt_coords.set( MT_SELX, MT_SELZ );
	set_normal_pattern( FORM_CUBE_TOP, normal );
	set_metatexture_coordinates_pattern( FORM_CUBE_TOP, mt_coords );

	normal[0] = normal[1] = normal[2] = normal[3] = cml::vector3f( 0.0f, -1.0f, 0.0f );
	mt_coords.set( MT_SELX, MT_SELZ );
	set_normal_pattern( FORM_CUBE_BOTTOM, normal );
	set_metatexture_coordinates_pattern( FORM_CUBE_BOTTOM, mt_coords );
}

void split_textures() {
	int index;

#define image_pixel_width 2048.0f

//	float su,sv;
	float u[4];
	float v[4];
	float w[4];

	cml::vector3f texture[4];

	for( index = 0; index < 256; index++ ) {
		w[0] = (float)index ;
		w[1] = w[0];
		w[2] = w[0];
		w[3] = w[0];

		u[0] = 4.0f / 128.0f;
		u[1] = u[0];
		u[2] = 124.0f / 128.0f;
		u[3] = u[2];

		v[0] = 4.0f / 128.0f;
		v[1] = 124.0f / 128.0f;
		v[2] = v[1];
		v[3] = v[0];
		
		/*
		v = 4.0f / 128.0f;

		su = (float)(index % 16) / 16.0f ;
		sv = (float)(index / 16) / 16.0f ;

		u[0] = su + 4.0f / image_pixel_width;
		u[1] = u[0];
		u[2] = su + 124.0f / image_pixel_width;
		u[3] = u[2];

		v[0] = sv + 4.0f / image_pixel_width;
		v[1] = sv + 124.0f / image_pixel_width;
		v[2] = v[1];
		v[3] = v[0];
		*/

		for( int i = 0; i < 4; i++ )
			texture[i] = cml::vector3f( u[i], v[i], w[i] );

		set_texture_pattern( index, texture );
		
		
	}
	
	texture[0] = cml::vector3f( 0.0f, 0.0f, 0.0f );
	texture[1] = cml::vector3f( 0.0f, 1.0f, 0.0f );
	texture[2] = cml::vector3f( 1.0f, 1.0f, 0.0f );
	texture[3] = cml::vector3f( 1.0f, 0.0f, 0.0f );
	set_texture_pattern( TEXTURE_WATER, texture );
}

void set_context( planet::context *ct ) {
	skylight_manager.ChangeContext( ct );

}

void initialize_patterns() {
	cml::vector3f default_normal_vector(0,0,0);
	cml::vector3f default_vertex(0,0,0);
	cml::vector3f default_uv(0,0,0);
	cml::vector2f default_mt_selection(0,0);
	cml::vector2f default_mt_size(1,1);

	cml::vector3f default_form[4];
	default_form[0] = default_vertex;
	default_form[1] = default_vertex;
	default_form[2] = default_vertex;
	default_form[3] = default_vertex;

	cml::vector3f default_texture[4];
	default_texture[0] = default_uv;
	default_texture[1] = default_uv;
	default_texture[2] = default_uv;
	default_texture[3] = default_uv;

	cml::vector3f default_normal[4];
	default_normal[0] = default_normal_vector;
	default_normal[1] = default_normal_vector;
	default_normal[2] = default_normal_vector;
	default_normal[3] = default_normal_vector;

	for( int i = 0; i < 8192; i++ ) {
		set_form_pattern( i, default_form );
		set_normal_pattern( i, default_normal );
		set_texture_pattern( i, default_texture );
		set_metatexture_coordinates_pattern( i, default_mt_selection );
		set_metatexture_dimensions_pattern( i, default_mt_size );
	}
}

void setup_metatexture_pattern( int index, int texture_start, int mtx, int mty ) {
	float w = (float)texture_start;
	cml::vector3f uvw[4];
	uvw[0].set( 4.0f / 128.0f, 4.0f / 128.0f, w );
	uvw[1].set( 4.0f / 128.0f, 124.0f / 128.0f, w );
	uvw[2].set( 124.0f / 128.0f, 124.0f / 128.0f, w );
	uvw[3].set( 124.0f / 128.0f, 4.0f / 128.0f, w );
	set_texture_pattern( index, uvw );
	set_metatexture_dimensions_pattern( index, cml::vector2f( (float)mtx, (float)mty ) );
}

void build_special_textures() {
	setup_metatexture_pattern( 0, world_texture::HD_GRASS, 4, 4 );
	setup_metatexture_pattern( 2, world_texture::HD_DIRT, 4, 4 );
	setup_metatexture_pattern( 20, world_texture::HD_ROCK, 4, 4 );
}

void init() {

	skylight_manager.Init();
	for( int i = 0; i < 16*16; i++ ) {
		empty_skylight_data[i] = 0x7fff;
	}
	 
	vertex_buffer.Create( Video::VF_TERRAIN, instanceCount );

	for( int i = 0; i < instanceCount; i++ ) {
		instances[i].registered    = false;
		//instances[i].data_ref	   = 0;
		instances[i].map_index     = -1;
		instances[i].index		   = i;
		instances[i].vbosize	   = 0;
		instances[i].vbosize_water = 0;
	}

	for( int i = 0; i < RIM_WIDTH*RIM_WIDTH*RIM_HEIGHT; i++ ) {
		rendering_instance_map[i] = 0;
	}

	Textures::CreateEmptyTextureF32( Textures::TERRAIN_PATTERNS, 16, 8192 );
	

	initialize_patterns();
	build_simple_forms();
	build_simple_normals();
	
	split_textures();
	build_special_textures();

	
	set_empty_stamp(0);

	// setup stamps
	//stamps[0].size			= 0;
	//stamps[0].start_vertex	= 0;
	set_cube_stamp( 1,   3,  3,  3,  3,  0,  2 ); // grass
	set_cube_stamp( 2,   2,  2,  2,  2,  2,  2 ); // dirt
	set_cube_stamp( 3,   18, 18, 18, 18, 18, 18 ); // sand

	// ::4::
	
	set_cube_stamp( 4,   20, 20, 20, 20, 20, 20 ); // rock
	set_cube_stamp( 5,   16, 16, 16, 16, 19, 19 ); // tree trunk
	
	set_cube_stamp( 6,   3,  3,  3,  3,  1,  2 ); // forest grass (todo)
	set_cube_stamp( 7,   5,  5,  5,  5,  5,  5 ); // bricks

	// ::8::

	set_cube_stamp( 8,   6,  6,  6,  6,  6,  6 ); // LIGHT-BLOCK-TEST
	set_cube_stamp( 9,   7,  7,  7,  7,  7,  7 ); // LIGHT-BLOCK-TEST
	set_cube_stamp( 10,  8,  8,  8,  8,  8,  8 ); // LIGHT-BLOCK-TEST
	set_cube_stamp( 11,  9,  9,  9,  9,  9,  9 ); // LIGHT-BLOCK-TEST

	// ::12::

	set_empty_stamp(12); // air

	{ // tallgrass
		stamp *grass = set_empty_stamp( 13 );
		grass->size = 2;
		grass->start_vertex = get_stamp_vertex_index(stampWrite);

		set_stamp_vertex( stampWrite, FORM_CROSS1, 5 );
		set_stamp_vertex( stampWrite+1, FORM_CROSS2, 5 );
		stampWrite+= 2;
		/*
		add_stamp(4*4);
		u16 u,v,w;
		u = CONVERTUV(512+4+128);
		v = CONVERTUV(0+4);
		w = CONVERTUV(120);

		add_vertex(0,1,0,u,v); // a
		add_vertex(0,0,0,u,v+w);
		add_vertex(1,0,1,u+w,v+w);
		add_vertex(1,1,1,u+w,v);

		add_vertex(0,1,1,u,v); // b
		add_vertex(0,0,1,u,v+w);
		add_vertex(1,0,0,u+w,v+w);
		add_vertex(1,1,0,u+w,v);

		add_vertex(0,1,0,u,v);
		add_vertex(1,1,1,u+w,v); // a backface
		add_vertex(1,0,1,u+w,v+w);
		add_vertex(0,0,0,u,v+w);

		add_vertex(0,1,1,u,v); // b backface
		add_vertex(1,1,0,u+w,v);
		add_vertex(1,0,0,u+w,v+w);
		add_vertex(0,0,1,u,v+w);*/
	}
	//add_empty_stamp();]
	set_cube_stamp( 15, 17, 17, 17, 17, 17, 17 );
	///add_cube_stamp(17,17,17,17,17,17);//add_cube_stamp(33,33,33,33,33,33);			// leaves

	// ::1:0::

	// mountains (16-19)
	for( int i = 0; i < 4; i++ ) {
		set_cube_stamp( 16+i, 20+i,20+i,20+i,20+i,20+i,20+i );
	}
	
	
	//add_empty_stamp();add_empty_stamp();add_empty_stamp();add_empty_stamp();
	//add_empty_stamp();add_empty_stamp();add_empty_stamp();add_empty_stamp();
	//add_empty_stamp();add_empty_stamp();add_empty_stamp();add_empty_stamp();
	//add_empty_stamp();
	//add_cube_stamp(128,128,128,128,128,128);	// water

	// ::1:13::

	set_cube_stamp( 29,  256,256,256,256,256,256 ); // water 
	
	/*

	for( int i = 0; i < 4; i++ ) {
		stampWrite[i-8].y -= 0.1;
	}
 
	add_empty_stamp();
	add_empty_stamp();*/
	
	// prime lighting buffers
	for( int i = 0; i < TOTALF; i++ ) {
		bg_data_buffer[i] = 0;
	}
	update_light_uniforms=true;
}

}

#endif
