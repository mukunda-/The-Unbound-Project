//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//

#pragma once

#if 0

//-------------------------------------------------------------------------------------------------

#include "video/shader.h"

//-------------------------------------------------------------------------------------------------
namespace Shaders {
	
template <size_t dimensions> class AttributePosition : public virtual Video::Shader {

	// Shader Vertex Attribute:
	// FLOAT X,[Y,[Z]]

	GLint a_position;

public:
	AttributePosition() {
		AddAttribute( a_position, "position" );
	}

	void VAP( int offset, int stride = (dimensions*4) ) {
		// offset = offset into vertex data
		// stride = total size of vertex data

		glVertexAttribPointer( a_position, dimensions, GL_FLOAT, GL_FALSE, stride, (void*)offset );
	}
	
};

//-------------------------------------------------------------------------------------------------
class BasicSampler : public virtual Video::Shader {

	GLint u_sampler;

public:
	BasicSampler() { 
		AddUniform( u_sampler, "texture_sampler" );
	}
	void SetSampler( int sampler ) {
		glUniform1i( u_sampler, sampler );
	}
};

//-------------------------------------------------------------------------------------------------
class BackdropShader :  
	public AttributePosition<2>,
	public BasicSampler {
	
private:
	
	//GLint u_sampler;
	GLint u_noise_sampler;
	//GLint planetRotation;

	GLint u_uyrot;
	GLint u_uxrot;

	GLint u_near;
	//GLint near_b;
	//GLint near_c;
	//GLint near_d;

	GLint u_far1;
	GLint u_far2;
	GLint u_far3;
	GLint u_far4;

	GLint u_sky_u;

	
	//GLint a_position;
public:
	BackdropShader();

	void SetVertexAttributePointers( int offset = 0, int set = 0 ) override;

	//void SetSampler( int sampler );
	void SetRotation( float angle, float pitch );
	void SetSkyU( float skyu );
	void SetShaderPlanes( Eigen::Vector3f &p_near, Eigen::Vector3f *p_far );
};

}


#endif
