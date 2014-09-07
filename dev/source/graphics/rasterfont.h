//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//
#pragma once

#include "video/textures.h"

namespace Graphics { 
	 
class Font  {

public:
	
	struct Character {
		uint16_t x; // texture coordinates
		uint16_t y;
		int16_t advance;
		uint8_t w; // width 
		uint8_t h; // height
		int8_t left;
		int8_t top;

		Character() {
		}
		
		Character( Character &copy ) {
			memcpy( this, &copy, sizeof Character );
		}
	};
	
	struct Pixel {
		uint8_t b;
		uint8_t g;
		uint8_t r;
		uint8_t a;
	};

	struct DrawingRect {
		int x;
		int y;
		int w;
		int h;
	};

	class CharacterSet {

	private:
		int height;
		int stroke;
		Character chars[96];

	public:
		CharacterSet( Character *set, int height, int stroke );

		const Character *GetCharacter( char code ) const;
		int GetHeight() const ;
		int GetStroke() const ;
		bool Matches( int height, int stroke ) const;
	};


private:
	std::unique_ptr<Pixel[]> pixels;
	//Pixel pixels[512*512];
	int raster_x, raster_y;
	int raster_largest_height;
	
	void CopyBitmap( FT_Bitmap &source, DrawingRect &rt );
	void CopyBitmapStroked( FT_Bitmap &source, DrawingRect &rt, int strokepen );
	void OverlayBitmap( FT_Bitmap &source, DrawingRect &rt );

	void InitFreeType();

public:
	Font();
	~Font();

	std::vector<std::unique_ptr<CharacterSet> > charsets;

	bool LoadFace( const char *filename, int height, int stroke = 0 );
	const char *GetBitmap() const;
	
	const CharacterSet *GetCharacterSet( int height, int stroke = 0 ) const ;

	void LoadTexture( Video::Texture &texture );
	void LoadTexture( Video::Texture::Pointer &texture );

	void DebugDump() const;
};

}  // Graphics
