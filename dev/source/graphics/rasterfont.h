//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2015, Mukunda Johnson, All rights reserved. =========//

#pragma once

#include "video/forwards.h"

//-----------------------------------------------------------------------------
namespace Graphics { 
	 
/** ---------------------------------------------------------------------------
 * A Font reads font files and generates font bitmaps for rendering use.
 */
class Font  {

public:
	
	/** -----------------------------------------------------------------------
	 * Position and size of a single character in the font bitmap.
	 */
	struct Character {
		uint16_t x; // texture coordinates
		uint16_t y;
		int16_t  advance;
		uint8_t  w; // width 
		uint8_t  h; // height
		int8_t   left;
		int8_t   top;

		Character() {
		}
		
		Character( Character &copy ) = default;
	};
	
	/** -----------------------------------------------------------------------
	 * A BGRA pixel.
	 */
	struct Pixel {
		uint8_t b;
		uint8_t g;
		uint8_t r;
		uint8_t a;
	};

	/** -----------------------------------------------------------------------
	 * A rect.
	 */
	struct DrawingRect {
		int x;
		int y;
		int w;
		int h;
	};

	/** -----------------------------------------------------------------------
	 * A complete array of (important) ascii characters.
	 */
	class CharacterSet {

	private:
		int       m_height;
		int       m_stroke;
		Character m_chars[96];

	public:
		CharacterSet( Character *set, int height, int stroke );

		const Character *GetCharacter( char code ) const;
		int GetHeight() const { return m_height; }
		int GetStroke() const { return m_stroke; }
		bool Matches( int height, int stroke ) const { 
			return m_height == height && m_stroke == stroke; 
		}

	};
	 
private:

	//-------------------------------------------------------------------------
	std::unique_ptr<Pixel[]> pixels; // the output bitmap
	//Pixel pixels[512*512];
	int raster_x, raster_y;          // the current raster position

	// variable to keep track of the tallest character, used when resetting
	// to the next line in the bitmap
	int raster_largest_height;
	
	void CopyBitmap( FT_Bitmap &source, DrawingRect &rt );
	void CopyBitmapStroked( FT_Bitmap &source, DrawingRect &rt, int strokepen );
	void OverlayBitmap( FT_Bitmap &source, DrawingRect &rt );
	  
public:
	Font();
	virtual ~Font();

	std::vector<std::unique_ptr<CharacterSet> > charsets;

	bool LoadFace( const char *filename, int height, int stroke = 0 );
	const char *GetBitmap() const;
	
	const CharacterSet *GetCharacterSet( int height, int stroke = 0 ) const ;

	void LoadTexture( Video::Texture &texture );
	void LoadTexture( Video::TexturePtr &texture );

	void DebugDump() const;
};

}  // Graphics
