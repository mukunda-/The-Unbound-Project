//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//

#include "stdafx.h"

//-------------------------------------------------------------------------------------------------
namespace Video { namespace Shaders {

//-------------------------------------------------------------------------------------------------
ObjectShader::ObjectShader( const char *name ) {
	AddShader( "glsl\\objectshader.v.glsl", GL_VERTEX_SHADER );
	AddShader( "glsl\\objectshader.f.glsl", GL_FRAGMENT_SHADER );
	Link();

	AddUniform( u_translate, "translate" );
	AddUniform( u_sampler, "textureSampler" );
	AddUniform( u_camera, "camera" );

	AddUniform( u_fog_distance, "fogDistance" );
	AddUniform( u_fog_length, "fogLength" );

	AddUniform( u_skylight_color, "sunlight_tint" );
	AddUniform( u_skylight_intensity, "sunlight_intensity" );

	AddAttribute( a_position, "position" );
	AddAttribute( a_texcoord, "texcoord" );
	AddAttribute( a_color, "colour" );

	Register( name );
}

//-------------------------------------------------------------------------------------------------
void ObjectShader::SetVertexAttributePointers( int offset, int set ) const {
	glVertexAttribPointer( a_texcoord, 2, GL_FLOAT, GL_FALSE, 24, (void*)(offset+0) );
	glVertexAttribPointer( a_color, 4, GL_UNSIGNED_BYTE, GL_TRUE, 24, (void*)(offset+8) );
	glVertexAttribPointer( a_position, 3, GL_FLOAT, GL_FALSE, 24, (void*)(offset+12) );

}
	
//-------------------------------------------------------------------------------------------------
void ObjectShader::SetTranslation( float x, float y, float z ) const {
	glUniform3f( u_translate, x, y, z );
}
//-------------------------------------------------------------------------------------------------
void ObjectShader::SetSampler( int sampler ) const {
	glUniform1i( u_sampler, sampler );
}
//-------------------------------------------------------------------------------------------------
void ObjectShader::SetSkylight( float r, float g, float b, float i ) const {
	glUniform3f( u_skylight_color, r, g, b );
	glUniform1f( u_skylight_intensity, i );
}
//-------------------------------------------------------------------------------------------------
void ObjectShader::SetFog( float distance, float length ) const {
	glUniform1f( u_fog_distance, distance );
	glUniform1f( u_fog_length, length );
}

//-------------------------------------------------------------------------------------------------
void ObjectShader::SetCameraMatrix( const float *matrix ) const {
	glUniformMatrix4fv( u_camera, 1, GL_FALSE, matrix );
}

}}

