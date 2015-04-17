//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2015, Mukunda Johnson, All rights reserved. =========//

#include "stdafx.h"
#include "util/stringles.h"
#include "linetester.h"

namespace Shaders {

//-----------------------------------------------------------------------------
void LineTester::Kernel::Param_Color( const Stref &value ) {
	// todo: parse color and set value
			
	Util::ParseColorString( value, m_color );
}

//-----------------------------------------------------------------------------
void LineTester::Kernel::ResetToDefault() {
	m_color.setOnes();
}

//-----------------------------------------------------------------------------
LineTester::Kernel::Kernel( std::shared_ptr<const KernelMap> &map ) : 
		Shader::Kernel( map ) { 

}

//-----------------------------------------------------------------------------
LineTester::LineTester() : Shader( "linetest" ) { 

	m_kernelmap = std::make_shared<KernelMap>();
	m_kernelmap->AddParam( "color", &Kernel::Param_Color );

	AddUniform( u_color, "color" );

	AddShader( "glsl\\linetest.v.glsl", GL_VERTEX_SHADER );
	AddShader( "glsl\\linetest.f.glsl", GL_FRAGMENT_SHADER );
	Link(); 
}

//-----------------------------------------------------------------------------
void LineTester::SetVertexAttributePointers( int offset, int set ) {
	AttributePosition::VAP( offset );
}

//-----------------------------------------------------------------------------
void LineTester::SetColor( const Eigen::Vector4f &color ) {
	glUniform3fv( u_color, 1, color.data() );
}

//-----------------------------------------------------------------------------
void LineTester::LoadKernel( Video::Shader::Kernel &kernel ) {
	auto &k = dynamic_cast<Kernel&>( kernel );
	SetColor( k.m_color );
}

//-----------------------------------------------------------------------------
std::shared_ptr<Video::Shader::Kernel> LineTester::CreateKernel() { 
	return std::make_shared<Kernel>( 
		std::static_pointer_cast<const KernelMap>( m_kernelmap ));
}

//-----------------------------------------------------------------------------
void LineTester::SetCamera() { 
	ShaderCamera::CopyCamera();
}
 
}
