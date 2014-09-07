//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//


#pragma once

//-------------------------------------------------------------------------------------------------
#ifndef VIDEO_WORLD_SHADER_H
#define VIDEO_WORLD_SHADER_H

//-------------------------------------------------------------------------------------------------
#include <gl/gl.h>
#include <gl/glu.h>
#include "video/shader.h"

//-------------------------------------------------------------------------------------------------
namespace Video { namespace Shaders {

//-------------------------------------------------------------------------------------------------
class WorldShader : public Shader {


	// uniforms
	GLint u_pattern_sampler;
	GLint u_noise_sampler;
	GLint u_global_translation;
	GLint u_camera;
	GLint u_skylight_color;
	GLint u_skylight_intensity;
	GLint u_fog_distance;
	GLint u_fog_length;

	GLint u_texture_sampler;
	GLint u_texture_translation;
	GLint u_opacity;
	GLint u_fog_color;

	GLint u_light_brightness;
	GLint u_light_colors;
	GLint u_light_positions;

	// vertex attributes
	GLint a_instance_translation;
	GLint a_instance_form;
	GLint a_instance_texture;
	GLint a_instance_color;
	
public:
	WorldShader( const char *name );
	void SetVertexAttributePointers( int offset = 0, int set = 0 ) const;
	
	void SetNoise( int texture_unit ) const;
	void SetPatterns( int texture_unit ) const;
	void SetTexturing( int texture_unit ) const;
	void SetFogArea( float distance, float length ) const;
	void SetFogColor( float r, float g, float b ) const;
	void SetSkylight( float r, float g, float b, float intensity ) const;
	void SetGlobalTranslation( float x, float y, float z ) const;
	void SetTextureTranslation( float u, float v, float w ) const;
	void SetOpacity( float alpha ) const;
	void SetLights( const float *positions, const float *colors, const float *brightness ) const;
	void SetCameraMatrix( const float *matrix ) const;

	enum {
		MAX_LIGHTS = 32
	};
};

}}

//-------------------------------------------------------------------------------------------------
#endif
