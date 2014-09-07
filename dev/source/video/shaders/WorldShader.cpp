//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//

//-------------------------------------------------------------------------------------------------
#include "stdafx.h"

//-------------------------------------------------------------------------------------------------
namespace Video { namespace Shaders {

//-------------------------------------------------------------------------------------------------
WorldShader::WorldShader( const char *name ) {
	AddShader( "glsl\\worldshader.v.glsl", GL_VERTEX_SHADER );
	AddShader( "glsl\\worldshader.f.glsl", GL_FRAGMENT_SHADER );
	Link();
	
	AddUniform( u_pattern_sampler, "sampler_patterns" );
	AddUniform( u_noise_sampler, "sampler_noise" );
	AddUniform( u_global_translation, "global_translation" );
	AddUniform( u_camera, "camera" );
	AddUniform( u_skylight_color, "skylight_color" );
	AddUniform( u_skylight_intensity, "skylight_intensity" );
	AddUniform( u_fog_distance, "fog_distance" );
	AddUniform( u_fog_length, "fog_length" );
	
	AddAttribute( a_instance_translation, "instance_translation", 1 );
	AddAttribute( a_instance_form, "instance_form", 1 );
	AddAttribute( a_instance_texture, "instance_texture", 1 );
	AddAttribute( a_instance_color, "instance_color", 1, 4 );
	//AddAttribute( attribInstanceColor2, "instance_color_2", 1 );
	//AddAttribute( attribInstanceColor3, "instance_color_3", 1 );
	//AddAttribute( attribInstanceColor4, "instance_color_4", 1 );

	AddUniform( u_texture_sampler, "sampler_texture" );
	AddUniform( u_texture_translation, "texture_translation" );
	AddUniform( u_opacity, "opacity" );
	AddUniform( u_fog_color, "fog_color" );

	AddUniform( u_light_brightness, "light_brightness" );
	AddUniform( u_light_colors, "light_colors" );
	AddUniform( u_light_positions, "light_positions" );

	Register( name );
}

//-------------------------------------------------------------------------------------------------
void WorldShader::SetVertexAttributePointers( int offset, int set ) const {
	if( set == 1 ) {
		glVertexAttribPointer( a_instance_translation, 1, GL_UNSIGNED_SHORT, GL_FALSE, 24, (void*)offset );
		glVertexAttribPointer( a_instance_form,        1, GL_UNSIGNED_SHORT, GL_FALSE, 24, (void*)(offset+2) );
		glVertexAttribPointer( a_instance_texture,     1, GL_UNSIGNED_SHORT, GL_FALSE, 24, (void*)(offset+4) );
		glVertexAttribPointer( a_instance_color+0,     4, GL_UNSIGNED_BYTE,  GL_TRUE,  24, (void*)(offset+8) );
		glVertexAttribPointer( a_instance_color+1,     4, GL_UNSIGNED_BYTE,  GL_TRUE,  24, (void*)(offset+12) );
		glVertexAttribPointer( a_instance_color+2,     4, GL_UNSIGNED_BYTE,  GL_TRUE,  24, (void*)(offset+16) );
		glVertexAttribPointer( a_instance_color+3,     4, GL_UNSIGNED_BYTE,  GL_TRUE,  24, (void*)(offset+20) );
	}
}
	
//-------------------------------------------------------------------------------------------------
void WorldShader::SetNoise( int texture_unit ) const {
	glUniform1i( u_noise_sampler, texture_unit );
}

//-------------------------------------------------------------------------------------------------
void WorldShader::SetPatterns( int texture_unit ) const {
	glUniform1i( u_pattern_sampler, texture_unit );
}

//-------------------------------------------------------------------------------------------------
void WorldShader::SetTexturing( int texture_unit ) const {
	glUniform1i( u_texture_sampler, texture_unit );
}

//-------------------------------------------------------------------------------------------------
void WorldShader::SetFogArea( float distance, float length ) const {
	glUniform1f( u_fog_distance, distance );
	glUniform1f( u_fog_length, length );
}

//-------------------------------------------------------------------------------------------------
void WorldShader::SetFogColor( float r, float g, float b ) const {
	glUniform3f( u_fog_color, r, g, b );
}

//-------------------------------------------------------------------------------------------------
void WorldShader::SetSkylight( float r, float g, float b, float intensity ) const {
	glUniform3f( u_skylight_color, r, g, b );
	glUniform1f( u_skylight_intensity, intensity );
}

//-------------------------------------------------------------------------------------------------
void WorldShader::SetGlobalTranslation( float x, float y, float z ) const {
	glUniform3f( u_global_translation, x, y, z );
}

//-------------------------------------------------------------------------------------------------
void WorldShader::SetTextureTranslation( float u, float v, float w ) const {
	glUniform3f( u_texture_translation, u, v, w );
}

//-------------------------------------------------------------------------------------------------
void WorldShader::SetOpacity( float alpha ) const {
	glUniform1f( u_opacity, alpha );
}

//-------------------------------------------------------------------------------------------------
void WorldShader::SetLights( const float *positions, const float *colors, const float *brightness ) const {
	glUniform3fv( u_light_positions, MAX_LIGHTS, positions );
	glUniform3fv( u_light_colors, MAX_LIGHTS, colors );
	glUniform1fv( u_light_brightness, MAX_LIGHTS, colors );
}

//-------------------------------------------------------------------------------------------------
void WorldShader::SetCameraMatrix( const float *matrix ) const {
	glUniformMatrix4fv( u_camera, 1, GL_FALSE, matrix );
}

//-------------------------------------------------------------------------------------------------
}}
