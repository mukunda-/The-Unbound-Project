//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//


#pragma once

//-------------------------------------------------------------------------------------------------
#ifndef VIDEO_SHADERS_SURFACEMIXER_H
#define VIDEO_SHADERS_SURFACEMIXER_H

//-------------------------------------------------------------------------------------------------
#include <gl/gl.h>
#include <gl/glu.h>
#include "video/shader.h"

//-------------------------------------------------------------------------------------------------
namespace Video { namespace Shaders {

//-------------------------------------------------------------------------------------------------
class SurfaceMixer : public Shader {

private:
	
	GLint a_position;

	GLint u_sampler1;
	GLint u_sampler2;
	
	GLint u_alpha;

public:
	SurfaceMixer( const char *name );
	void SetVertexAttributePointers( int offset = 0, int set = 0 ) const;
	
	void SetSamplers( int sampler1, int sampler2 ) const;
	void SetAlpha( float alpha ) const;
};

}}

//-------------------------------------------------------------------------------------------------
#endif
