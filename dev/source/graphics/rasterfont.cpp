//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//

#include "stdafx.h"
#include "video/textures.h"
#include "graphics/rasterfont.h"

namespace Graphics { 

#define padding 4
#define bitmap_width 512

FT_Library g_ftlib;
bool g_ft_init = false;

//-------------------------------------------------------------------------------------------------
Font::CharacterSet::CharacterSet( Character *set, int p_height, int p_stroke ) {
	for( int i = 0; i < 96; i++ ) {
		chars[i] = set[i];
	}
	height = p_height;
	stroke = p_stroke;
}

//-------------------------------------------------------------------------------------------------
int Font::CharacterSet::GetHeight() const {
	return height;
}

//-------------------------------------------------------------------------------------------------
int Font::CharacterSet::GetStroke() const {
	return stroke;
}

//-------------------------------------------------------------------------------------------------
bool Font::CharacterSet::Matches( int p_height, int p_stroke ) const {
	return ( height == p_height && stroke == p_stroke );
}

//-------------------------------------------------------------------------------------------------
const Font::Character * Font::CharacterSet::GetCharacter( char code ) const {
	if( code >= 32 && code <= 127 ) {
		return &chars[code-32];
	}
	return 0;
}

//-------------------------------------------------------------------------------------------------
void Font::InitFreeType() {
	if( g_ft_init ) return;
	int error = FT_Init_FreeType( &g_ftlib );
	if( error ) {
		printf( "error starting FreeType: %i\n", error );
	}
	g_ft_init = true;
}

//-------------------------------------------------------------------------------------------------
void Font::CopyBitmap( FT_Bitmap &source, DrawingRect &rt ) {
	
	for( int py = 0; py < source.rows; py++ ) {
		for( int px = 0; px < source.width; px++ ) {
			int tx = rt.x + px ;
			int ty = rt.y + py ;
			if( tx < rt.x ) continue;
			if( ty < rt.y ) continue;
			if( tx >= (rt.x+rt.w) ) continue;
			if( ty >= (rt.y+rt.h) ) continue;

			int pix = source.buffer[px+py*source.width];

			pixels[(tx+ty*bitmap_width)].r = 255;
			pixels[(tx+ty*bitmap_width)].g = 255;
			pixels[(tx+ty*bitmap_width)].b = 255;
			pixels[(tx+ty*bitmap_width)].a = pix;
			
		}	
	}
}

//-------------------------------------------------------------------------------------------------
void Font::OverlayBitmap( FT_Bitmap &source, DrawingRect &rt ) {
	
	for( int py = 0; py < source.rows; py++ ) {
		for( int px = 0; px < source.width; px++ ) {
			int tx = rt.x + px ;
			int ty = rt.y + py ;
			if( tx < rt.x ) continue;
			if( ty < rt.y ) continue;
			if( tx >= (rt.x+rt.w) ) continue;
			if( ty >= (rt.y+rt.h) ) continue;

			int pix = source.buffer[px+py*source.width];

			pixels[(tx+ty*bitmap_width)].r += pix;
			pixels[(tx+ty*bitmap_width)].g += pix;
			pixels[(tx+ty*bitmap_width)].b += pix;
			
		}	
	}
}

//-------------------------------------------------------------------------------------------------
void Font::CopyBitmapStroked( FT_Bitmap &source, DrawingRect &rt, int stroke ) {
	for( int py = 0; py < source.rows; py++ ) {
		for( int px = 0; px < source.width; px++ ) {
			int tx = rt.x + px ;
			int ty = rt.y + py ;
			if( tx < rt.x ) continue;
			if( ty < rt.y ) continue;
			if( tx >= (rt.x+rt.w) ) continue;
			if( ty >= (rt.y+rt.h) ) continue;

			int pix = source.buffer[px+py*source.width];
			 
			if( pix > 0 ) {
				for( int gy = -stroke; gy <= stroke; gy++ ) {
						
					for( int gx = -stroke; gx <= stroke; gx++ ) {
						 
						if( pix > pixels[(tx+gx)+(ty+gy)*bitmap_width].a ) { 
							pixels[(tx+gx)+(ty+gy)*bitmap_width].a = pix;
						} 
						
					}
				}
			}
			
		}	
	}

	OverlayBitmap( source, rt );

}

//-------------------------------------------------------------------------------------------------
bool Font::LoadFace( const char *filename, int height, int stroke ) {

	if( GetCharacterSet( height, stroke ) ) return false; // character set already loaded
 
	FT_Face face;
	FT_New_Face( g_ftlib, filename, 0, &face );
	
	FT_Set_Char_Size( face, 0, height*64, 72, 72 );

	Character chars[96];

	for( int i = 32; i < 128; i++ ) {
		FT_Load_Char( face, i, FT_LOAD_RENDER | FT_LOAD_FORCE_AUTOHINT );
		FT_GlyphSlot slot = face->glyph;
		

		DrawingRect rt;
		rt.x = raster_x;
		rt.y = raster_y;
		rt.w = slot->bitmap.width+stroke*2+1;
		rt.h = slot->bitmap.rows+stroke*2+1;
		if( rt.h > raster_largest_height ) raster_largest_height = rt.h;

		if( rt.x + rt.w >= 512 ) {
			raster_x = 0;
			raster_y += raster_largest_height;
			raster_largest_height = rt.h;

			rt.x = raster_x;
			rt.y = raster_y;
			if( rt.y + rt.h >= bitmap_width ) break; // out of texture space!
		}

		//rt.x+=1;
		//rt.y+=1;
		rt.x += stroke;
		rt.y += stroke;
		rt.w-=1+stroke*2;
		rt.h-=1+stroke*2;
		
		chars[i-32].x = rt.x - stroke;
		chars[i-32].y = rt.y - stroke;
		chars[i-32].w = rt.w + stroke*2;
		chars[i-32].h = rt.h + stroke*2;
		chars[i-32].left = slot->bitmap_left - stroke;
		chars[i-32].top  = slot->bitmap_top + stroke;
 
 		chars[i-32].advance = (short)slot->advance.x;
		if( !stroke )
			CopyBitmap( slot->bitmap, rt );
		else
			CopyBitmapStroked( slot->bitmap, rt, stroke );

		raster_x += slot->bitmap.width+1+stroke*2;
	}

	charsets.push_back( std::unique_ptr<CharacterSet>(new CharacterSet( chars, height, stroke )) );

	return true;
	
	
}

//-------------------------------------------------------------------------------------------------
void Font::DebugDump() const {
	boost::uint8_t debug_bmp[bitmap_width*bitmap_width*3];
	for( int py = 0; py < bitmap_width; py++ ) {
		for( int px = 0; px < bitmap_width; px++ ) {
			int a = pixels[px+py*bitmap_width].a;
			int t = pixels[px+py*bitmap_width].r;
			t = 255 * (255-a) + t * a;
			t /= 255;
			debug_bmp[(px+py*bitmap_width)*3] = t;
			debug_bmp[(px+py*bitmap_width)*3+1] = t;
			debug_bmp[(px+py*bitmap_width)*3+2] = t;
		}
	}
	//debug_dump( bitmap_width,bitmap_width,debug_bmp);
}

//-------------------------------------------------------------------------------------------------
const Font::CharacterSet * Font::GetCharacterSet( int height, int stroke ) const {

	for( boost::uint32_t i = 0; i < charsets.size(); i++ ) {
		if( charsets[i]->Matches(height,stroke) ) {
			return charsets[i].get();
		}
	}
	return nullptr;
}

//-------------------------------------------------------------------------------------------------
const char *Font::GetBitmap() const {
	return (const char*)pixels.get();
}

//-------------------------------------------------------------------------------------------------
void Font::LoadTexture( Video::Texture &texture ) {
	texture.Create( Video::Texture::FORMAT_RGBA8, bitmap_width, bitmap_width, 1, 1 );
	texture.Load( GetBitmap() );
}

//-------------------------------------------------------------------------------------------------
void Font::LoadTexture( Video::Texture::Pointer &texture ) {
	LoadTexture( *texture );
}

//-------------------------------------------------------------------------------------------------
Font::Font() {
	InitFreeType();
	pixels = std::unique_ptr<Pixel[]>( new Pixel[512*512] );
	raster_x=0;
	raster_y=0;
	raster_largest_height=0;
	for( int i = 0; i < 512*512; i++ ) {
		pixels[i].r = pixels[i].g = pixels[i].b = pixels[i].a = 0;
	}
}

//-------------------------------------------------------------------------------------------------
Font::~Font() {
	 
}

} // namespaces
