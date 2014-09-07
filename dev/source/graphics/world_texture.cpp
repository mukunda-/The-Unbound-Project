//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//

#include "stdafx.h"

#if 0
namespace world_texture {

void load_main_tiles() {
	stb_image img( "texture\\tiles.tga" );

	unsigned char *swizzled;
	swizzled = new unsigned char[128*128*4];

	
	
	for( int i = 0; i < 256; i++ ) {
		int sx,sy;
		sx = i % 16;
		sy = i / 16;

		for( int x = 0; x < 128; x++ ) {
			for( int y = 0; y < 128; y++ ) {

				for( int p = 0; p < 4; p++ ) {
					
					swizzled[(y*128 + x)*4+p] = img.data()[((sx*128+x)+(sy*128+y)*img.width())*4+p];
				}
			}
		}

		Textures::TextureArraySubImageRGBA( Textures::WORLD, 0, 0, i, 128,128, 1, swizzled );
	
	}

	delete[] swizzled;
}

void add_metatexture( int slot, const char *filename, int mtx, int mty ) {
	stb_image img( filename );

	unsigned char *tile;
	tile = new unsigned char[128*128*4];

	for( int y = 0; y < mty; y++ ) {
		for( int x = 0; x < mtx; x++ ) {
			
			for( int py = 0; py < 128; py++ ) {
				for( int px = 0; px < 128; px++ ) {

					for( int i = 0; i < 4; i++ ) {
						tile[(px+py*128)*4+i] = img.data()[ (x * 128 + px + ((y*128)+py) * img.width()) *4 + i];
					}
				}
			}
			
			Textures::TextureArraySubImageRGBA( Textures::WORLD, 0, 0, slot, 128, 128, 1, tile );
			slot++;
		}
	}

	delete[] tile;
}

void create() {
	
	Textures::CreateEmptyTextureArrayRGBA( Textures::WORLD, 128, 128, 512 );
	load_main_tiles();

	add_metatexture( HD_GRASS, "texture\\grass.tga", 4, 4 );
	add_metatexture( HD_DIRT, "texture\\dirt.tga", 4, 4 );
	add_metatexture( HD_ROCK, "texture\\rock.tga", 4, 4 );

	Textures::SetTextureAnisotropy( Textures::WORLD, Textures::GetMaxAnisotropy() );
	Textures::AutoTextureMipmap( Textures::WORLD );
	Textures::SetMagFilter( Textures::WORLD, Textures::FILTER_NEAREST );

	//Textures::LoadTextureArrayFromMemoryRGBA( Textures::WORLD, swizzled, 128,128,256 );
	
	
	
	
}

}

#endif
