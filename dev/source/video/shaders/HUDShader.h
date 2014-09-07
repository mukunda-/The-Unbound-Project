//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//

//-------------------------------------------------------------------------------------------------
#pragma once

//-------------------------------------------------------------------------------------------------
#ifndef VIDEO_SHADERS_HUDSHADER_H
#define VIDEO_SHADERS_HUDSHADER_H

#include <gl/gl.h>
#include <gl/glu.h>
#include "video/shader.h"

//-------------------------------------------------------------------------------------------------
namespace Video { namespace Shaders {

//-------------------------------------------------------------------------------------------------
class HUDShader : public Shader {

	GLint u_translate;
	GLint u_sampler;
	GLint a_position;
	GLint a_texcoord;
	GLint a_color;

public:
	HUDShader( const char *name );
	
	void SetVertexAttributePointers( int offset = 0, int set = 0 ) const;

	void SetTranslation( float x, float y ) const;
	void SetSampler( int texture_unit ) const;

};

}}

//-------------------------------------------------------------------------------------------------
#endif
