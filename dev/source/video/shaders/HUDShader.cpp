//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//

#include "stdafx.h"

//-------------------------------------------------------------------------------------------------
namespace Video { namespace Shaders {

//-------------------------------------------------------------------------------------------------
HUDShader::HUDShader( const char *name ) {
	AddShader( "glsl\\hudshader.v.glsl", GL_VERTEX_SHADER );
	AddShader( "glsl\\hudshader.f.glsl", GL_FRAGMENT_SHADER );
	Link();

	AddUniform( u_translate, "translate" );
	AddUniform( u_sampler, "textureSampler" );

	AddAttribute( a_position, "position" );
	AddAttribute( a_texcoord, "texcoord" );
	AddAttribute( a_color, "color" );

	Register( name );
}

//-------------------------------------------------------------------------------------------------
void HUDShader::SetVertexAttributePointers( int offset, int set ) const {
	
	glVertexAttribPointer( a_texcoord, 2, GL_FLOAT, GL_FALSE, 20, (void*)(offset+0) );
	glVertexAttribPointer( a_color, 4, GL_UNSIGNED_BYTE, GL_TRUE, 20, (void*)(offset+8) );
	glVertexAttribPointer( a_position, 2, GL_FLOAT, GL_FALSE, 20, (void*)(offset+12) );
}

//-------------------------------------------------------------------------------------------------
void HUDShader::SetTranslation( float x, float y ) const {
	glUniform2f( u_translate, x, y );
}

//-------------------------------------------------------------------------------------------------
void HUDShader::SetSampler( int texture_unit ) const {
	glUniform1i( u_sampler, texture_unit );
}

}}

//-------------------------------------------------------------------------------------------------
