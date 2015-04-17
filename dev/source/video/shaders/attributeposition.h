//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2015, Mukunda Johnson, All rights reserved. =========//

#pragma once

#include "video/shader.h"

//-----------------------------------------------------------------------------
namespace Shaders {

/** ---------------------------------------------------------------------------
 * Shader helper template that handles a "position" attribute.
 *
 * @param T The implementing class; must be derived from Shader.
 * @param dimensions Number of dimensions, or components in the position.
 */
template <class T, size_t dimensions> 
class AttributePosition {

	// Shader Vertex Attribute:
	// FLOAT X,[Y,[Z]]

protected:
	GLint a_position;
	
	AttributePosition() {
		static_cast<T*>(this)->AddAttribute( a_position, "position" );
	}

public:
	
	void VAP( int offset, int stride = (dimensions*4) ) {
		// offset = offset into vertex data
		// stride = total size of vertex data

		glVertexAttribPointer( a_position, dimensions, GL_FLOAT, GL_FALSE, 
			                   stride, (void*)offset );
	}
	
};

}
