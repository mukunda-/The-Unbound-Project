//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2015, Mukunda Johnson, All rights reserved. =========//

#pragma once

#include "util/stref.h"
#include "video/forwards.h" 
#include "video/shader.h"

namespace Graphics {

//-----------------------------------------------------------------------------
/** Materials are a combination of rendering settings used for rendering an
 *  element.
 */
class Material {

	enum {
		// max number of textures that may be used for a single material
		TEXTURE_SLOTS = 4
	};

	// shader to use for rendering
	Video::Shader *shader; 

	// shader kernel settings to pass to shader
	std::shared_ptr<Video::Shader::Kernel> kernel; 

	// texture handles for the textures used
	Video::TexturePtr textures[TEXTURE_SLOTS];


public:
	Material( const Stref &shader_name );
	virtual ~Material() {}

	/** -----------------------------------------------------------------------
	 * Set a shader parameter.
	 *
	 * @param name Name of parameter to set.
	 * @param value Value to pass to shader kernel. 
	 */
	void SetParam( const Stref &name, const Stref &value ); 

	/** -----------------------------------------------------------------------
	 * Set one of the material's textures.
	 *
	 * @param slot [0,TEXTURE_SLOTS) slot of texture to set.
	 * @param tex Texture handle
	 */
	void SetTexture( int slot, const Video::TexturePtr &tex );

	/** -----------------------------------------------------------------------
	 * Get the handle to one of the textures used in this material.
	 *
	 * @param slot [0,TEXTURE_SLOTS) slot of the texture to read
	 * @returns the texture handle in the slot, or nullptr if none.
	 */
	Video::TexturePtr GetTexture( int slot );

	/** -----------------------------------------------------------------------
	 * Bind this texture to the GL context for rendering.
	 */
	void Bind();

	//--------------------------------------------------------------------------
	using ptr = std::shared_ptr<Material>;
};

}