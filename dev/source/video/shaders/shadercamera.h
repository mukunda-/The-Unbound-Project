//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

#pragma once

#include "video/shader.h"

//-----------------------------------------------------------------------------
namespace Shaders {

/** ---------------------------------------------------------------------------
 * A helper class to handle copying the video camera into a uniform variable
 * named "camera".
 */
class ShaderCamera : public virtual Video::Shader {

	GLint u_camera;
	int   m_camera_serial;

public:
	ShaderCamera() {
		AddUniform( u_camera, "camera" );
		m_camera_serial = 0;
	}

	/** -----------------------------------------------------------------------
	 * Call this in Shader::SetCamera()
	 */
	void CopyCamera() { 
		int serial = Video::GetXPMatrixSerial();
		if( serial == camera_serial ) return;
		camera_serial = serial;

		glUniformMatrix4fv( u_camera, 1, GL_FALSE, 
				Video::GetXPMatrix().data() );
	}
	
}; 

}