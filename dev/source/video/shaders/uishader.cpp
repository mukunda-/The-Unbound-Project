//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

#include "stdafx.h"
#include "uishader.h"

//-----------------------------------------------------------------------------
namespace Shaders {

//-----------------------------------------------------------------------------
Ui::Ui() : Shader( "ui" ) {
	AddUniform( u_sampler, "u_sampler" );
		
	AddAttribute( a_position, "a_position" );
	AddAttribute( a_texcoord, "a_texcoord" );
	AddAttribute( a_color,    "a_color"    );

	AddShader( "glsl\\ui.v.glsl", GL_VERTEX_SHADER   );
	AddShader( "glsl\\ui.f.glsl", GL_FRAGMENT_SHADER );
	Link();

	glUniform1i( u_sampler, 0 );
}

//-----------------------------------------------------------------------------
void Ui::SetVertexAttributePointers( int offset, int set ) {

	glVertexAttribPointer( a_position, 2, GL_FLOAT, GL_FALSE, 20,          (void*)offset      );
	glVertexAttribPointer( a_texcoord, 2, GL_FLOAT, GL_FALSE, 20,          (void*)(offset+8)  );
	glVertexAttribPointer( a_color,    4, GL_UNSIGNED_BYTE,   GL_TRUE, 20, (void*)(offset+16) );
}

//-----------------------------------------------------------------------------
void Ui::LoadKernel( Video::Shader::Kernel &pkernel ) {
	Kernel &kernel = dynamic_cast<Kernel&>( pkernel ); 
}

//-----------------------------------------------------------------------------
auto Ui::CreateKernel() -> std::shared_ptr<Shader::Kernel> { 
	return std::make_shared<Kernel>( 
		std::static_pointer_cast<const KernelMap>( m_kernelmap ));
}

//-----------------------------------------------------------------------------
}