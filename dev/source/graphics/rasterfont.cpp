//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2015, Mukunda Johnson, All rights reserved. =========//

#include "stdafx.h"
#include "video/texture.h"
#include "rasterfont.h"
#include "graphics.h"

#include "debug/dumpbitmap.h"

//-----------------------------------------------------------------------------
namespace Graphics { 

enum {
	PADDING = 4,
	BITMAP_WIDTH = 512
};

//-----------------------------------------------------------------------------
Font::CharacterSet::CharacterSet( Character *set, int height, int stroke ) {
	for( int i = 0; i < 96; i++ ) {
		m_chars[i] = set[i];
	}
	m_height = height;
	m_stroke = stroke;
}

//-----------------------------------------------------------------------------
const Font::Character * Font::CharacterSet::GetCharacter( char code ) const {
	if( code >= 32 && code <= 127 ) {
		return &m_chars[code-32];
	}
	return nullptr;
}

//-----------------------------------------------------------------------------
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

			pixels[(tx+ty*BITMAP_WIDTH)].r = 255;
			pixels[(tx+ty*BITMAP_WIDTH)].g = 255;
			pixels[(tx+ty*BITMAP_WIDTH)].b = 255;
			pixels[(tx+ty*BITMAP_WIDTH)].a = pix;
			
		}	
	}
}

//-----------------------------------------------------------------------------
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

			pixels[(tx+ty*BITMAP_WIDTH)].r += pix;
			pixels[(tx+ty*BITMAP_WIDTH)].g += pix;
			pixels[(tx+ty*BITMAP_WIDTH)].b += pix;
			
		}	
	}
}

//-----------------------------------------------------------------------------
void Font::CopyBitmapStroked( FT_Bitmap &source, DrawingRect &rt, 
	                          int stroke ) {

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
						 
						if( pix > pixels[(tx+gx)+(ty+gy)*BITMAP_WIDTH].a ) { 
							pixels[(tx+gx)+(ty+gy)*BITMAP_WIDTH].a = pix;
						} 
						
					}
				}
			}
			
		}	
	}

	OverlayBitmap( source, rt );

}

//-----------------------------------------------------------------------------
bool Font::LoadFace( const char *filename, int height, int stroke ) {

	if( GetCharacterSet( height, stroke ) ) {
		return false; // character set already loaded
	}
 
	FT_Face face;
	FT_New_Face( Graphics::FTLib(), filename, 0, &face );
	
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
			if( rt.y + rt.h >= BITMAP_WIDTH ) break; // out of texture space!
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

	// free memory
	FT_Done_Face( face );

	charsets.push_back( std::unique_ptr<CharacterSet>( new CharacterSet( chars, height, stroke )));

	return true; 
}

//-------------------------------------------------------------------------------------------------
void Font::DebugDump() const {
	boost::uint8_t debug_bmp[BITMAP_WIDTH*BITMAP_WIDTH*3];
	for( int py = 0; py < BITMAP_WIDTH; py++ ) {
		for( int px = 0; px < BITMAP_WIDTH; px++ ) {
			int a = pixels[px+py*BITMAP_WIDTH].a;
			int t = pixels[px+py*BITMAP_WIDTH].r;
			t = 255 * (255-a) + t * a;
			t /= 255;
			debug_bmp[(px+py*BITMAP_WIDTH)*3] = t;
			debug_bmp[(px+py*BITMAP_WIDTH)*3+1] = t;
			debug_bmp[(px+py*BITMAP_WIDTH)*3+2] = t;
		}
	}
	Debug::DumpBitmap( BITMAP_WIDTH, BITMAP_WIDTH, debug_bmp );
}

//-----------------------------------------------------------------------------
const Font::CharacterSet * Font::GetCharacterSet( 
									int height, int stroke ) const {

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

//-----------------------------------------------------------------------------
void Font::LoadTexture( Video::Texture &texture ) {
	texture.Create( Video::Texture::Format::RGBA8, 
		            BITMAP_WIDTH, BITMAP_WIDTH, 1, 1 );
	texture.Load( GetBitmap() );
}

//-----------------------------------------------------------------------------
void Font::LoadTexture( Video::Texture::ptr &texture ) {
	LoadTexture( *texture );
}

//-----------------------------------------------------------------------------
Font::Font() {

	pixels = std::unique_ptr<Pixel[]>( 
					new Pixel[BITMAP_WIDTH * BITMAP_WIDTH] );
	raster_x = 0;
	raster_y = 0;
	raster_largest_height = 0;

	memset( (void*)pixels.get(), 0, BITMAP_WIDTH*BITMAP_WIDTH*4 );
}

//-------------------------------------------------------------------------------------------------
Font::~Font() {
	 
}

} // namespaces
