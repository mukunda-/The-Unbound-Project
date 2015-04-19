//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2015, Mukunda Johnson, All rights reserved. =========//

#include "stdafx.h"
#include "material.h"
#include "video/texture.h"
#include "video/video.h"

//-----------------------------------------------------------------------------
namespace Graphics {

//-----------------------------------------------------------------------------
Material::Material( const Stref &shader_name ) {
	shader = Video::FindShader( shader_name );
	if( !shader ) {
		//shader = Videos::Shaders::Default(); (todo)
		throw std::runtime_error( "Invalid shader used for material." );
	}

	kernel = shader->CreateKernel();
}

//-----------------------------------------------------------------------------
void Material::SetTexture( int slot, const Video::Texture::ptr &tex ) {
	textures[slot] = tex;
}

//-----------------------------------------------------------------------------
Video::Texture::ptr Material::GetTexture( int slot ) {
	return textures[slot];
}

//-----------------------------------------------------------------------------
void Material::SetParam( const Stref &name, const Stref &value ) {
	/*if( name == "texture" ) {
		SetTexture( 0, value );
		return;
	} else if( name == "texture2" ) {
		SetTexture( 1, value );
		return;
	} else if( name == "texture3" ) {
		SetTexture( 2, value );
		return;
	} else if( name == "texture4" ) {
		SetTexture( 3, value );
		return;
	}*/
	kernel->SetParam( name, value );
}

//-----------------------------------------------------------------------------
void Material::Bind() {
	shader->Use();
	shader->LoadKernel( *kernel );

	if( textures[0].get() != nullptr ) {
		textures[0]->Bind();
	}
}

//-----------------------------------------------------------------------------
}
