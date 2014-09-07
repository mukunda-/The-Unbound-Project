//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//

#include "stdafx.h"

namespace Video { namespace Shaders {

//-------------------------------------------------------------------------------------------------
SurfaceMixer::SurfaceMixer( const char *name ) {

	AddShader( "glsl\\screenshader.v.glsl", GL_VERTEX_SHADER );
	AddShader( "glsl\\surfacemixer.f.glsl", GL_FRAGMENT_SHADER );

	Link();

	AddUniform( u_sampler1, "sampler1" );
	AddUniform( u_sampler2, "sampler2" );
	AddUniform( u_alpha, "alpha" );
	AddAttribute( a_position, "position" );
	
	Register( name );
}

//-------------------------------------------------------------------------------------------------
void SurfaceMixer::SetVertexAttributePointers( int offset, int set ) const {
	glVertexAttribPointer( a_position, 2, GL_FLOAT, GL_FALSE, 8, (void*)offset );

}
	
//-------------------------------------------------------------------------------------------------
void SurfaceMixer::SetSamplers( int sampler1, int sampler2 ) const {
	glUniform1i( u_sampler1, sampler1 );
	glUniform1i( u_sampler2, sampler2 );
}

//-------------------------------------------------------------------------------------------------
void SurfaceMixer::SetAlpha( float alpha ) const {
	glUniform1f( u_alpha, alpha );
}

}}
