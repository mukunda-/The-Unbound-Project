//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2016, Mukunda Johnson, All rights reserved. ==========//

//-------------------------------------------------------------------------------------------------
#pragma once

//-------------------------------------------------------------------------------------------------
#ifndef VIDEO_OBJECT_SHADER_H
#define VIDEO_OBJECT_SHADER_H

//-------------------------------------------------------------------------------------------------
#include <gl/gl.h>
#include <gl/glu.h>
#include "video/shader.h"

//-------------------------------------------------------------------------------------------------
namespace Video { namespace Shaders {

//-------------------------------------------------------------------------------------------------
class ObjectShader : public Shader {

private:
	GLint u_translate;
	GLint u_sampler;
	GLint u_camera;
	GLint u_fog_distance;
	GLint u_fog_length;

	GLint u_skylight_color;
	GLint u_skylight_intensity;

	GLint a_position;
	GLint a_texcoord;
	GLint a_color;
public:

	ObjectShader( const char *name );
	void SetVertexAttributePointers( int offset = 0, int set = 0 ) const;
	
	void SetTranslation( float x, float y, float z ) const;
	void SetSampler( int sampler ) const;
	void SetSkylight( float r, float g, float b, float i ) const;
	void SetFog( float distance, float length ) const;
	void SetCameraMatrix( const float *matrix ) const;
};

//-------------------------------------------------------------------------------------------------
} }

//-------------------------------------------------------------------------------------------------
#endif
