//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//

#pragma once

#include "graphics/graphics.h"
#include "graphics/rasterfont.h"

namespace Graphics {


class FontMaterial : public Graphics::Font {

public:
	//Font m_font;
	Material m_material; 

	FontMaterial(): m_material( "ui" ) {
		m_material.SetTexture( 0, Video::Texture::New() );
	}

	void LoadMaterial() {
		LoadTexture( *m_material.GetTexture(0) );
	}
};

}
