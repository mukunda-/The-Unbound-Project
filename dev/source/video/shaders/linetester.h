//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2015, Mukunda Johnson, All rights reserved. =========//

#pragma once

//-----------------------------------------------------------------------------

#include "video/shader.h"
#include "video/shaders/backdropshader.h"
#include "shadercamera.h"
#include "attributeposition.h"

namespace Shaders {

/** ---------------------------------------------------------------------------
 * A test shader.
 */
class LineTester :    
	public Video::Shader,
	public AttributePosition<LineTester,3>,
	public ShaderCamera<LineTester> {

	friend class AttributePosition<LineTester,3>;
	friend class ShaderCamera<LineTester>;
	
public:

	//-------------------------------------------------------------------------
	class Kernel : public Video::Shader::Kernel {
		
		friend class LineTester;

		Eigen::Vector4f m_color;
		
		void Param_Color( const Stref &value );

	public:
		EIGEN_MAKE_ALIGNED_OPERATOR_NEW

		void ResetToDefault() override;

		Kernel( std::shared_ptr<const KernelMap> &map ); 
	};

private:
	
	GLint u_color;
	std::shared_ptr<KernelMap> m_kernelmap;

public:

	//-------------------------------------------------------------------------
	LineTester();
	 
	/** -----------------------------------------------------------------------
	 * Set the color to render the lines.
	 *
	 * @param color r,g,b,a components in range [0.0,1.0]
	 */
	void SetColor( const Eigen::Vector4f &color ); 

	//-------------------------------------------------------------------------
	void SetVertexAttributePointers( int offset, int set ) override; 
	void LoadKernel( Video::Shader::Kernel &pkernel ); 
	std::shared_ptr<Shader::Kernel> CreateKernel() override; 
	void SetCamera();
};

}