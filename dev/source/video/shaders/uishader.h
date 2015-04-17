//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//

#pragma once
 
#include "video/shader.h"

//-------------------------------------------------------------------------------------------------
namespace Shaders {
 
class Ui : public Video::Shader {

	
public:
	//-------------------------------------------------------------------------
	class Kernel : public Video::Shader::Kernel {
	
		friend class Ui;
		
	public:
		EIGEN_MAKE_ALIGNED_OPERATOR_NEW
			 
		void ResetToDefault() override {
				
		}

		Kernel( std::shared_ptr<const KernelMap> &map ) : Shader::Kernel( map ) {
			
		}
	};

private:
	
	GLint u_sampler;
	GLint a_position;
	GLint a_texcoord;
	GLint a_color;

	std::shared_ptr<KernelMap> m_kernelmap;

public:

	//-------------------------------------------------------------------------
	Ui() : Shader( "ui" ) { 
		AddUniform( u_sampler, "sampler" );
		
		AddAttribute( a_position, "position" );
		AddAttribute( a_texcoord, "texcoord" );
		AddAttribute( a_color, "color" );

		AddShader( "glsl\\ui.v.glsl", GL_VERTEX_SHADER );
		AddShader( "glsl\\ui.f.glsl", GL_FRAGMENT_SHADER );
		Link();

		glUniform1i( u_sampler, 0 );
	}
	
	//-------------------------------------------------------------------------
	void SetVertexAttributePointers( int offset, int set ) override {

		glVertexAttribPointer( a_position, 2, GL_FLOAT, GL_FALSE, 20, (void*)offset );
		glVertexAttribPointer( a_texcoord, 2, GL_FLOAT, GL_FALSE, 20, (void*)(offset+8) );
		glVertexAttribPointer( a_color, 4, GL_UNSIGNED_BYTE, GL_TRUE, 20, (void*)(offset+16) );
	} 
	
	//-------------------------------------------------------------------------
	void LoadKernel( Video::Shader::Kernel &pkernel ) override {
		Kernel &kernel = dynamic_cast<Kernel&>( pkernel ); 

	}
	
	//-------------------------------------------------------------------------
	std::shared_ptr<Shader::Kernel> CreateKernel() override { 
		return std::make_shared<Kernel>( 
			std::static_pointer_cast<const KernelMap>( m_kernelmap ));
	}
	  
};

}