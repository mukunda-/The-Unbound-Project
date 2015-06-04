//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

#pragma once

#include "video/shader.h"

//-----------------------------------------------------------------------------
namespace Shaders {
 
//-----------------------------------------------------------------------------
class Ui : public Video::Shader {

public:
	//-------------------------------------------------------------------------
	class Kernel : public Video::Shader::Kernel {
	
		friend class Ui;
		
	public:
		EIGEN_MAKE_ALIGNED_OPERATOR_NEW
		
		void ResetToDefault() override {}
		
		Kernel( std::shared_ptr<const KernelMap> &map ) : Shader::Kernel( map ) {
			
		}
	};

	//-------------------------------------------------------------------------
private:
	
	GLint u_sampler;
	GLint a_position;
	GLint a_texcoord;
	GLint a_color;

	std::shared_ptr<KernelMap> m_kernelmap;

public:

	//-------------------------------------------------------------------------
	Ui();
	
	//-------------------------------------------------------------------------
	void SetVertexAttributePointers( int offset, int set ) override;
	void LoadKernel( Video::Shader::Kernel &pkernel ) override;
	std::shared_ptr<Shader::Kernel> CreateKernel() override;
};

}