//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//
#include "stdafx.h"
 

namespace objects {

/*
#define maxvert 65536

obj_vertex vertex_buffer[maxvert];
int buffer_write;
int buffer_size;


obj_vertex vertex_buffer_nonblended[maxvert];
int buffer_nb_write;
int buffer_nb_size;

cml::vector3f viewplane_h;
cml::vector3f viewplane_v;

GLuint vboID[2];

float sine[256];

GLuint test_vbo;
int test_vbo_size;

void add_vertex( float x, float y, float z, float u, float v, u8 r, u8 g, u8 b, u8 a, bool blend ) {
	obj_vertex *w;
	if( blend ) {
		if( buffer_write >= maxvert ) return;
		w = vertex_buffer+buffer_write;
		buffer_write++;
	} else {
		if( buffer_nb_write >= maxvert ) return;
		w = vertex_buffer_nonblended+buffer_nb_write;
		buffer_nb_write++;
	}
	w->x = x;
	w->y = y;
	w->z = z;
	w->u = u;
	w->v = v;
	w->r = r;
	w->g = g;
	w->b = b;
	w->a = a;
}


void compute_viewing_planes() {
	viewplane_h = Video::farPlane[3] - Video::farPlane[0];
	viewplane_v = Video::farPlane[0] - Video::farPlane[1];

	viewplane_h.normalize();
	viewplane_v.normalize();
}

float tablecos( int rot ) {
	return sine[(64-rot)&255];
}

float tablesin( int rot ) {
	return sine[rot&255];
}

void draw_billboard_blended( cml::vector3f position, float size, float u1, float v1, float u2, float v2, u8 lum, int rot ) {

	
	size *= 0.5;
	
	cml::vector3f vertices[4];
	u1 += 0.001f;
	u2 -= 0.001f;
	v1 += 0.001f;
	v2 -= 0.001f;

	float c, s;

	c =tablecos(rot);
	s =tablesin(rot);

	float x1 = c * -size - s * size;
	float y1 = s * -size + c * size;

	float x2 = c * -size - s * -size;
	float y2 = s * -size + c * -size;

	float x3 = c * size - s * -size;
	float y3 = s * size + c * -size;

	float x4 = c * size - s * size;
	float y4 = s * size + c * size;

	vertices[0] = position + (x1 * viewplane_h) + (y1 * viewplane_v);
	vertices[1] = position + (x2 * viewplane_h) + (y2 * viewplane_v);
	vertices[2] = position + (x3 * viewplane_h) + (y3 * viewplane_v);
	vertices[3] = position + (x4 * viewplane_h) + (y4 * viewplane_v);
	

	add_vertex( vertices[0][0], vertices[0][1], vertices[0][2], u1, v1, lum,lum,lum,0, true );
	add_vertex( vertices[1][0], vertices[1][1], vertices[1][2], u1, v2, lum,lum,lum,0,true );
	add_vertex( vertices[2][0], vertices[2][1], vertices[2][2], u2, v2, lum,lum,lum,0,true );

	add_vertex( vertices[2][0], vertices[2][1], vertices[2][2], u2, v2, lum,lum,lum,0,true );
	add_vertex( vertices[3][0], vertices[3][1], vertices[3][2], u2, v1, lum,lum,lum,0,true );
	add_vertex( vertices[0][0], vertices[0][1], vertices[0][2], u1, v1, lum,lum,lum,0,true );
}

void draw_billboard_solid( cml::vector3f position, float size, float u1, float v1, float u2, float v2, u8 lum, int rot ) {

	u32 light = worldrender::get_light_value( (int)floor(position[0]), (int)floor(position[1])+1, (int)floor(position[2]) );
	u8 r = (((u8)light)*lum)>>8 ;
	u8 g = (((u8)(light>>8))*lum)>>8;
	u8 b = (((u8)(light>>16))*lum)>>8 ;
	u8 a = ((((u8)(light>>24)) * 255 / 16)*lum)>>8;
	
	
	size *= 0.5;
	cml::vector3f vertices[4];
	u1 += 0.001f;
	u2 -= 0.001f;
	v1 += 0.001f;
	v2 -= 0.001f;

	float c, s;

	c =tablecos(rot);
	s =tablesin(rot);

	float x1 = c * -size - s * size;
	float y1 = s * -size + c * size;

	float x2 = c * -size - s * -size;
	float y2 = s * -size + c * -size;

	float x3 = c * size - s * -size;
	float y3 = s * size + c * -size;

	float x4 = c * size - s * size;
	float y4 = s * size + c * size;

	x1 = -size;
	y1 = size;
	x2=-size;
	y2=-size;
	x3=size;
	y3=-size;
	x4=size;
	y4=size;

	vertices[0] = position + (x1 * viewplane_h) + (y1 * viewplane_v);
	vertices[1] = position + (x2 * viewplane_h) + (y2 * viewplane_v);
	vertices[2] = position + (x3 * viewplane_h) + (y3 * viewplane_v);
	vertices[3] = position + (x4 * viewplane_h) + (y4 * viewplane_v);


	
	add_vertex( vertices[0][0], vertices[0][1], vertices[0][2], u1, v1, r,g,b,a,false );
	add_vertex( vertices[1][0], vertices[1][1], vertices[1][2], u1, v2, r,g,b,a,false );
	add_vertex( vertices[2][0], vertices[2][1], vertices[2][2], u2, v2, r,g,b,a,false );

	add_vertex( vertices[2][0], vertices[2][1], vertices[2][2], u2, v2, r,g,b,a,false );
	add_vertex( vertices[3][0], vertices[3][1], vertices[3][2], u2, v1, r,g,b,a,false );
	add_vertex( vertices[0][0], vertices[0][1], vertices[0][2], u1, v1, r,g,b,a,false );
}

void draw_billboard_ex( cml::vector3f position, float size, float u1, float v1, float u2, float v2, u8 lum, bool blend, int rot ) {
	if( blend ) {
		draw_billboard_blended( position, size, u1, v1, u2, v2, lum, rot );
	} else {
		draw_billboard_solid( position, size, u1, v1, u2, v2, lum, rot );
	}
}

void load_vertex_data() {
	// copy data to buffer
	glBindBufferARB( GL_ARRAY_BUFFER_ARB, vboID[0] );
	glBufferDataARB( GL_ARRAY_BUFFER_ARB, buffer_write * sizeof(obj_vertex), vertex_buffer, GL_STATIC_DRAW_ARB );
	buffer_size = buffer_write;
	buffer_write = 0;

	glBindBufferARB( GL_ARRAY_BUFFER_ARB, vboID[1] );
	glBufferDataARB( GL_ARRAY_BUFFER_ARB, buffer_nb_write * sizeof(obj_vertex), vertex_buffer_nonblended, GL_STATIC_DRAW_ARB );
	buffer_nb_size = buffer_nb_write;
	buffer_nb_write = 0;
}

void init() {
	
	glGenBuffersARB( 2, vboID );
	buffer_write= 0;
	buffer_size = 0;
	
	buffer_nb_write=0;
	buffer_nb_size=0;

	for( int i =0 ; i < 256; i++ ) {
		sine[i] = (float)sin( ((double)i / 256.0) * 6.283185307179586476925286766559 );
	}

	glGenBuffersARB( 1, &test_vbo );
}

void debug1( Models::Model &m ) {
	test_vbo_size = m.ReadVertexData( test_vbo );
}

*/

/*

void render_geometry_solid() {
	// render data
	
	Shaders::OS.Use();
	
	//glCullFace(GL_FRONT);
	glEnableVertexAttribArray( Shaders::OS.attribPosition );
	glEnableVertexAttribArray( Shaders::OS.attribTexCoord );
	glEnableVertexAttribArray( Shaders::OS.attribColour );
	
	glUniform1i( Shaders::OS.attribSampler, 0 );
	glUniform1f( Shaders::OS.attribFogDistance, Video::farPlaneZ() );
	glUniform1f( Shaders::OS.attribFogLength, Video::get_fog_length() );
	glUniform3f( Shaders::OS.attribTranslate, 0.0f, 0.0f, 0.0f );
	worldrender::setup_shader_sun( Shaders::OS.attribSunlightTint, Shaders::OS.attribSunlightIntensity );

	Video::set_camera_uniform( Shaders::OS.attribCamera );
	Textures::Bind(Textures::WORLD);

	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	
	glDepthMask(true);

	glBindBufferARB(GL_ARRAY_BUFFER_ARB, vboID[1] );
 	glVertexAttribPointer( Shaders::OS.attribTexCoord, 2, GL_FLOAT, GL_FALSE, 24, (void*)0 );
	glVertexAttribPointer( Shaders::OS.attribColour, 4, GL_UNSIGNED_BYTE, GL_TRUE, 24, (void*)8 );
	glVertexAttribPointer( Shaders::OS.attribPosition, 3, GL_FLOAT, GL_FALSE, 24, (void*)12 );
	glDrawArrays(GL_TRIANGLES, 0, buffer_nb_size );
	 
	
	glDisableVertexAttribArray( Shaders::OS.attribPosition );
	glDisableVertexAttribArray( Shaders::OS.attribTexCoord );
	glDisableVertexAttribArray( Shaders::OS.attribColour );
		
	// release vbo
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);

	
}

void render_geometry_additive_ex( GLuint buffer, int texture, GLuint nverts, GLenum rastermode ) {
	// render data
	
	Shaders::OS.Use();
	
	//glCullFace(GL_FRONT);
	glEnableVertexAttribArray( Shaders::OS.attribPosition );
	glEnableVertexAttribArray( Shaders::OS.attribTexCoord );
	glEnableVertexAttribArray( Shaders::OS.attribColour );
	
	glUniform1i( Shaders::OS.attribSampler, 0 );
	glUniform1f( Shaders::OS.attribFogDistance, Video::farPlaneZ() );
	glUniform1f( Shaders::OS.attribFogLength, Video::get_fog_length() );
	glUniform3f( Shaders::OS.attribTranslate, 0.0f, 0.0f, 0.0f );
	worldrender::setup_shader_sun( Shaders::OS.attribSunlightTint, Shaders::OS.attribSunlightIntensity );

	Video::set_camera_uniform( Shaders::OS.attribCamera );
	Textures::Bind(texture);

	//glCullFace(GL_FRONT);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);

	glDepthMask(false);
	
		
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, buffer );
 	glVertexAttribPointer( Shaders::OS.attribTexCoord, 2, GL_FLOAT, GL_FALSE, 24, (void*)0 );
	glVertexAttribPointer( Shaders::OS.attribColour, 4, GL_UNSIGNED_BYTE, GL_TRUE, 24, (void*)8 );
	glVertexAttribPointer( Shaders::OS.attribPosition, 3, GL_FLOAT, GL_FALSE, 24, (void*)12 );
	glDrawArrays( rastermode, 0, nverts );

	
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	
	glDepthMask(true);
	
	glDisableVertexAttribArray( Shaders::OS.attribPosition );
	glDisableVertexAttribArray( Shaders::OS.attribTexCoord );
	glDisableVertexAttribArray( Shaders::OS.attribColour );
		
	// release vbo
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);

	
	//glCullFace(GL_BACK);
}

void render_geometry_additive() {
	render_geometry_additive_ex( vboID[0], 0, buffer_size, GL_TRIANGLES );

}

*/

}