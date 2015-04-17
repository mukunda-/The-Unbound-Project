//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//

#include "stdafx.h"

#if 0

//-------------------------------------------------------------------------------------------------
namespace Video { namespace Shaders {

//-------------------------------------------------------------------------------------------------
CloudShader::CloudShader( const char *name ) {
	AddShader( "glsl\\cloudshader.v.glsl", GL_VERTEX_SHADER );
	AddShader( "glsl\\cloudshader.f.glsl", GL_FRAGMENT_SHADER );

	Link();

	
	AddAttribute( a_position, "position" );
	//AddAttribute( attribTexCoord, "texcoord" );
	//AddAttribute( attribColor, "color" );

	AddUniform( u_sampler1, "sampler1" );
	AddUniform( u_sampler2, "sampler2" );
	AddUniform( u_mu, "mu" );
	AddUniform( u_density, "density" );
	AddUniform( u_color, "tint" );

	//AddUniform( uniformCamera, "camera" );
	//AddUniform( uniformColor, "tint" );
	//AddUniform( uniformTranslate, "translate" );

	AddUniform( u_eye, "eye" );
	AddUniform( u_top_left, "topleft" );
	AddUniform( u_top_right, "topright" );
	AddUniform( u_bottom_left, "bottomleft" );
	AddUniform( u_bottom_right, "bottomright" );

	AddUniform( u_texture_offset, "texture_offset" );

	Register( name );
}

//-------------------------------------------------------------------------------------------------
void CloudShader::SetVertexAttributePointers( int offset, int set ) const {
	glVertexAttribPointer( a_position, 2, GL_FLOAT, GL_FALSE, 8, (void*)offset );
}

//-------------------------------------------------------------------------------------------------
void CloudShader::SetPlanes( cml::vector3f &p_near, cml::vector3f *p_far ) const {
	if( u_eye == -1 || u_top_left == -1 || u_bottom_left == -1 || u_bottom_right == -1 || u_top_right == -1 ) return;
	glUniform3f( u_eye, p_near[0], p_near[1], p_near[2]);//Video::nearPlane[0][1], Video::nearPlane[0][2] );
	//glUniform3f( Shaders::BS.near_b, Video::nearPlane[1][0], Video::nearPlane[1][1], Video::nearPlane[1][2] );
	//glUniform3f( Shaders::BS.near_c, Video::nearPlane[2][0], Video::nearPlane[2][1], Video::nearPlane[2][2] );
	//glUniform3f( Shaders::BS.near_d, Video::nearPlane[3][0], Video::nearPlane[3][1], Video::nearPlane[3][2] );
	glUniform3f( u_top_left,     p_far[0][0], p_far[0][1], p_far[0][2] );
	glUniform3f( u_bottom_left,  p_far[1][0], p_far[1][1], p_far[1][2] );
	glUniform3f( u_bottom_right, p_far[2][0], p_far[2][1], p_far[2][2] );
	glUniform3f( u_top_right,    p_far[3][0], p_far[3][1], p_far[3][2] );

}

//-------------------------------------------------------------------------------------------------
void CloudShader::SetSamplers( int sampler1, int sampler2 ) const {
	if( u_sampler1 != -1 ) glUniform1i( u_sampler1, sampler1 );
	if( u_sampler2 != -1 ) glUniform1i( u_sampler2, sampler2 );
}

//-------------------------------------------------------------------------------------------------
void CloudShader::SetMu( float mu ) const {
	if( u_mu != -1 ) glUniform1f( u_mu, mu );
}

//-------------------------------------------------------------------------------------------------
void CloudShader::SetDensity( float density ) const {
	if( u_density != -1 ) glUniform1f( u_density, density );
}

//-------------------------------------------------------------------------------------------------
void CloudShader::SetColor( float r, float g, float b ) const {
	if( u_color != -1 ) glUniform3f( u_color, r, g, b );
}

//-------------------------------------------------------------------------------------------------
void CloudShader::SetTextureOffset( float x, float y ) const {
	if( u_texture_offset != -1 ) glUniform2f( u_texture_offset, x, y );
}

//-------------------------------------------------------------------------------------------------
}}

#endif