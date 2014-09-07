//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//

#pragma once

//-------------------------------------------------------------------------------------------------
#ifndef VIDEO_SCREENSHADER_H
#define VIDEO_SCREENSHADER_H

//-------------------------------------------------------------------------------------------------
#include <gl/gl.h>
#include <gl/glu.h>
#include "video/shader.h"

//-------------------------------------------------------------------------------------------------
namespace Video { namespace Shaders {

//-------------------------------------------------------------------------------------------------
class ScreenShader : public Shader {
	
private:
	
	GLint u_sampler;
	GLint u_noise_sampler;
	GLint u_time;
	GLint a_position;
	
	GLint u_frag_sine_params;
	GLint u_screen_zoom;
	GLint u_horizontal_blur;
	GLint u_color_overlay;
	GLint u_distort;
	GLint u_color_multiply;

public:
	ScreenShader( const char *name );
	void SetVertexAttributePointers( int offset = 0, int set = 0 ) const;

	void SetSamplers( int texture, int noise ) const;
	void SetSineEffect( float frequency, float depth, float offset ) const;
	void SetColorOverlay( float r, float g, float b, float a ) const;
	void SetHorizontalBlur( float power ) const;
	void SetZoom( float zoom ) const;
	void SetDistortion( float distort ) const;
	void SetTime( float time ) const;
	void SetColorMultiply( float a ) const;
	
};

}}

//-------------------------------------------------------------------------------------------------
#endif
