//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2015, Mukunda Johnson, All rights reserved. ==========//

#pragma once

//-------------------------------------------------------------------------------------------------
#ifndef VIDEO_CLOUDSHADER_H
#define VIDEO_CLOUDSHADER_H

//-------------------------------------------------------------------------------------------------
#include <gl/gl.h>
#include <gl/glu.h>
#include "video/shader.h"

//-------------------------------------------------------------------------------------------------
namespace Video { namespace Shaders {

//-------------------------------------------------------------------------------------------------
class CloudShader : public Shader {
private:

	GLint a_position;
	//GLint attribTexCoord;
	//GLint attribColor;

	GLint u_eye;
	GLint u_top_left;
	GLint u_top_right;
	GLint u_bottom_left;
	GLint u_bottom_right;

	//GLint uniformCamera;
	//GLint uniformTranslate;

	GLint u_sampler1;
	GLint u_sampler2;
	GLint u_mu;
	GLint u_density;
	GLint u_color;

	GLint u_texture_offset;
public:
	CloudShader( const char *name );
	void SetVertexAttributePointers( int offset = 0, int set = 0 ) const;

	void SetPlanes( cml::vector3f &p_near, cml::vector3f *p_far ) const;
	void SetSamplers( int sampler1, int sampler2 ) const;
	void SetMu( float mu ) const;
	void SetDensity( float density ) const;
	void SetColor( float r, float g, float b ) const;
	void SetTextureOffset( float x, float y ) const;
	
};

//-------------------------------------------------------------------------------------------------
}}


//-------------------------------------------------------------------------------------------------
#endif
