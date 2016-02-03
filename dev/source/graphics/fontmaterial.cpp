//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2015, Mukunda Johnson, All rights reserved. =========//

#include "stdafx.h"
#include "fontmaterial.h"
#include "material.h"

namespace Graphics {

//-----------------------------------------------------------------------------
FontMaterial::FontMaterial()
	: 
	m_material( Graphics::CreateMaterial( "", "ui", 
		[]( Graphics::MaterialPtr mat, const Stref & ) {
			mat->SetTexture( 0, Video::Texture::New() );
		})
	) {

	//m_material->SetTexture( 0, Video::Texture::New() );
}

//-----------------------------------------------------------------------------
void FontMaterial::LoadMaterial() {
	LoadTexture( (*m_material).GetTexture(0) );
}

}