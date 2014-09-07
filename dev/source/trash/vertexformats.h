//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//

//-------------------------------------------------------------------------------------------------
#pragma once

//-------------------------------------------------------------------------------------------------
#ifndef VIDEO_VERTEXFORMATS_H
#define VIDEO_VERTEXFORMATS_H

//-------------------------------------------------------------------------------------------------
namespace Video {

//-------------------------------------------------------------------------------------------------
enum {
	VF_WORLD,
	VF_HUD,
	VF_GENERIC2D,
	VF_GENERIC,
	VF_POINT2D
};

//-------------------------------------------------------------------------------------------------
// world instance vertex
// for the world shader
//  
typedef struct t_world_instance_vertex {
	GLushort translation;
	GLushort form;
	GLushort texture;
	GLushort reserved;
	GLulong colors[4];
	
} world_instance_vertex;

//-------------------------------------------------------------------------------------------------
// generic 2d vertex
// position+color+texture for 2d geometry
//
typedef struct t_generic_vertex2d {
	float u;
	float v;
	u8 r;
	u8 g;
	u8 b;
	u8 a;
	float x;
	float y;
} generic_vertex2d;

//-------------------------------------------------------------------------------------------------
// generic vetex
// position+color+texture for geometry
//
typedef struct t_generic_vertex {
	float u;
	float v;
	u8 r;
	u8 g;
	u8 b;
	u8 a;
	float x;
	float y;
	float z;
} generic_vertex;

//-------------------------------------------------------------------------------------------------
// 2d point vertex
// position only for simple 2d operations
// such as fullscreen shaders
//
typedef struct t_point_vertex_2d {	
	float x;
	float y;
} point_vertex_2d;

//-------------------------------------------------------------------------------------------------

static inline generic_vertex SetupGenericVertex( float x = 0.0f, float y = 0.0f, float z = 0.0f, u8 r = 255, u8 g = 255, u8 b = 255, u8 a = 255, float u = 0.0f, float v = 0.0f ) {
	generic_vertex data;
	data.x = x;
	data.y = y;
	data.z = z;
	data.r = r;
	data.g = g;
	data.b = b;
	data.a = a;
	data.u = u;
	data.v = v;
	return data;
}

}

//-------------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------------
