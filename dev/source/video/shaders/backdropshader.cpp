//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//

#include "stdafx.h"
#include "include_game.h"

//-------------------------------------------------------------------------------------------------
namespace Shaders {
	 
//-------------------------------------------------------------------------------------------------
BackdropShader::BackdropShader() : Shader("backdrop") {

	//AddAttribute( a_position, "position" );
	//AddUniform( u_sampler, "texture_sampler" );
	AddUniform( u_noise_sampler, "noise_sampler" );
	AddUniform( u_uyrot, "yrot" );
	AddUniform( u_uxrot, "xrot" );
	AddUniform( u_near, "near_a" );
	
	AddUniform( u_far1, "far_a" );
	AddUniform( u_far2, "far_b" );
	AddUniform( u_far3, "far_c" );
	AddUniform( u_far4, "far_d" );
	AddUniform( u_sky_u, "skyu" );

	AddShader( "glsl\\backdrop.v.glsl", GL_VERTEX_SHADER );
	AddShader( "glsl\\backdrop.f.glsl", GL_FRAGMENT_SHADER );
	Link();

	glUniform1i( u_noise_sampler, 1 );

	Register();
}

//-------------------------------------------------------------------------------------------------
void BackdropShader::SetVertexAttributePointers( int offset, int set ) {
	AttributePosition::VAP( offset, 8 );
	//glVertexAttribPointer( a_position, 2, GL_FLOAT, GL_FALSE, 8, (void*)offset );
}

//-------------------------------------------------------------------------------------------------
//void BackdropShader::SetSampler( int sampler ) {
//	glUniform1i( u_sampler, sampler );
//}

//-------------------------------------------------------------------------------------------------
void BackdropShader::SetRotation( float angle, float pitch ) {
	glUniform1f( u_uyrot, angle );
	glUniform1f( u_uxrot, pitch );
}

//-------------------------------------------------------------------------------------------------
void BackdropShader::SetSkyU( float skyu ) {
	glUniform1f( u_sky_u, skyu );
}

//-------------------------------------------------------------------------------------------------
void BackdropShader::SetShaderPlanes( Eigen::Vector3f &p_near, Eigen::Vector3f *p_far ) {
	glUniform3f( u_near, p_near[0], p_near[1], p_near[2] );//
//	Video::nearPlane[0][1], Video::nearPlane[0][2] );
	//glUniform3f( Shaders::BS.near_b, Video::nearPlane[1][0], Video::nearPlane[1][1], Video::nearPlane[1][2] );
	//glUniform3f( Shaders::BS.near_c, Video::nearPlane[2][0], Video::nearPlane[2][1], Video::nearPlane[2][2] );
	//glUniform3f( Shaders::BS.near_d, Video::nearPlane[3][0], Video::nearPlane[3][1], Video::nearPlane[3][2] );
	glUniform3f( u_far1, (*p_far)(0,0), (*p_far)(0,1), (*p_far)(0,2) );
	glUniform3f( u_far2, (*p_far)(1,0), (*p_far)(1,1), (*p_far)(1,2) );
	glUniform3f( u_far3, (*p_far)(2,0), (*p_far)(2,1), (*p_far)(2,2) );
	glUniform3f( u_far4, (*p_far)(3,0), (*p_far)(3,1), (*p_far)(3,2) );

}

}
