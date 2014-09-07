//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//

#include "stdafx.h"

namespace Video { namespace Shaders {

//-------------------------------------------------------------------------------------------------
ScreenShader::ScreenShader( const char *name ) {
	AddShader( "glsl\\screenshader.v.glsl", GL_VERTEX_SHADER );
	AddShader( "glsl\\screenshader.f.glsl", GL_FRAGMENT_SHADER );
	Link();
	
	AddUniform( u_sampler, "textureSampler" );
	AddUniform( u_noise_sampler, "noiseSampler" );
	AddUniform( u_time, "time" );
	AddUniform( u_frag_sine_params, "fragSineParams" );
	AddUniform( u_screen_zoom, "screenZoom" );
	AddUniform( u_color_overlay, "colorOverlay" );
	AddUniform( u_horizontal_blur, "horizontalBlur" );
	AddUniform( u_distort, "distort" );
	AddUniform( u_color_multiply, "colorMul" );

	AddAttribute( a_position, "position" );

	Register( name );
}

//-------------------------------------------------------------------------------------------------
void ScreenShader::SetVertexAttributePointers( int offset, int set ) const {
	glVertexAttribPointer( a_position, 2, GL_FLOAT, GL_FALSE, 8, (void*)offset );
	glVertexAttribDivisor( a_position, 0 );
}
	
//-------------------------------------------------------------------------------------------------
void ScreenShader::SetSamplers( int texture, int noise ) const {
	glUniform1i( u_sampler, texture );
	glUniform1i( u_noise_sampler, noise );
}
//-------------------------------------------------------------------------------------------------
void ScreenShader::SetSineEffect( float frequency, float depth, float offset ) const {
	glUniform3f( u_frag_sine_params, frequency, depth, offset );
}
//-------------------------------------------------------------------------------------------------
void ScreenShader::SetColorOverlay( float r, float g, float b, float a ) const {
	glUniform4f( u_color_overlay, r, g, b, a );
}
//-------------------------------------------------------------------------------------------------
void ScreenShader::SetHorizontalBlur( float power ) const {
	glUniform1f( u_horizontal_blur, power );
}
//-------------------------------------------------------------------------------------------------
void ScreenShader::SetZoom( float zoom ) const {
	glUniform1f( u_screen_zoom, zoom );
}
//-------------------------------------------------------------------------------------------------
void ScreenShader::SetDistortion( float distort ) const {
	glUniform1f( u_distort, distort );
}
//-------------------------------------------------------------------------------------------------
void ScreenShader::SetTime( float time ) const {
	glUniform1f( u_time, time );
}
//-------------------------------------------------------------------------------------------------
void ScreenShader::SetColorMultiply( float a ) const {
	glUniform1f( u_color_multiply, a );
}
	
//-------------------------------------------------------------------------------------------------
}}
