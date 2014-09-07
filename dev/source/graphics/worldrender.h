//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//

#if 0

#pragma once

#ifndef WORLDRENDER_H
#define WORLDRENDER_H

namespace worldrender {

void init();
void update();

void render();
void render_water();

void reset_instance( int index );

/*
typedef struct t_lightInstance {
	int x;
	int y;
	int z;
	int px, py, pz;
	int cr, cg, cb;
	int intensity;
	bool dirty; // (regenerate stamp)
	u8 stamp[32*32*32];
	
	struct t_lightInstance *next;
	struct t_lightInstance *prev;
} lightInstance;*/

typedef struct t_lightInstance2 {
	float x, y, z;
	float r, g, b;
	float brightness;
	bool dirty;

	struct t_lightInstance2 *next;
	struct t_lightInstance2 *prev;
} lightInstance2;

lightInstance2 * light_create( float brightness, float r, float g, float b );
void light_destroy( lightInstance2 *light );
void light_setpos( lightInstance2 *light, float x, float y, float z );

void terrain_particle( int x, int y, int z, int block );

void set_sunlight_color( float r, float g, float b );
void set_fog_color( float r, float g, float b );
void set_sunlight_intensity( float i );
void apply_sunlight( int &r, int &g, int &b, int sun );
void setup_shader_sun( int tint, int intensity );
//void set_sunlight_position( float angle );

cml::vector3f compute_light_mixed( float x, float y, float z );
u32 compute_light_7bit( float x, float y, float z );

void dirty_instance( int index, bool geometry, bool dimlight, bool skylight );

void dirty_area( int x, int y, int z, bool geometry, bool dimlight, bool skylight );
void dirty_pancake( int x, int y, int z, bool geometry, bool dimlight, bool skylight );

void set_skylight_dirty( planet::context *ct, int x, int y, int z );
void set_context( planet::context *ct );

};

#endif

#endif
