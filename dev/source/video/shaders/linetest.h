//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

#pragma once


//-----------------------------------------------------------------------------

#include "video/shader.h"
#include "video/shaders/backdropshader.h"
#include "shadercamera.h"

namespace Shaders {

/** ---------------------------------------------------------------------------
 * A test shader.
 */
class LineTester :  
	public AttributePosition<3>,
	public ShaderCamera,
	public virtual Video::Shader {
	
public:
	class Kernel : public Video::Shader::Kernel {
	
		friend class LineTester;

		Eigen::Vector4f color;
		
		void Param_Color( const char *value ) {
			// todo: parse color and set value
			
			Util::ParseColorString( value, color );
		}

	public:
		EIGEN_MAKE_ALIGNED_OPERATOR_NEW

		boost::shared_ptr<Video::Shader::Kernel> Create() override {
			return boost::shared_ptr<Video::Shader::Kernel>( new Kernel );
		}

		void ResetToDefault() override {
			color.setOnes();
		}

		Kernel() {
			AddParam( "color", boost::bind( &Kernel::Param_Color, this, _1 ) );
		}
	} kernel;

private:
	
	GLint u_color;


public:

	LineTester() : Shader( "linetest" ) { 
		AddUniform( u_color, "color" );

		AddShader( "glsl\\linetest.v.glsl", GL_VERTEX_SHADER );
		AddShader( "glsl\\linetest.f.glsl", GL_FRAGMENT_SHADER );
		Link();
		Register();
	}

	void SetVertexAttributePointers( int offset, int set ) override {
		AttributePosition::VAP( offset );
	}
	/*
	void SetColor( const Eigen::Vector4f &color ) {
		if( kernel.color != color ) {
			kernel.color = color;
			glUniform3fv( u_color, 1, color.data() );
		} 
	}*/

	void SetColor( const Eigen::Vector4f &color ) {
		glUniform3fv( u_color, 1, color.data() );
	}

	void LoadKernel( Video::Shader::Kernel &pkernel ) override {
		Kernel &kernel = dynamic_cast<Kernel&>(pkernel);
		SetColor( kernel.color );
	}

	boost::shared_ptr<Shader::Kernel> CreateKernel() override { 
		return kernel.Create();
	}
	 
	void SetCamera() override  { 
		ShaderCamera::SetCamera();
	}
};

}