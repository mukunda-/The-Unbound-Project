//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2015, Mukunda Johnson, All rights reserved. =========//

#pragma once

#include "util/stref.h"

//-----------------------------------------------------------------------------
namespace Video {

//-----------------------------------------------------------------------------
class Texture : public std::enable_shared_from_this<Texture> {

public:
	typedef std::shared_ptr<Texture> ptr;

	//-------------------------------------------------------------------------
	enum class Format : int {
		AUTO,    // automatically determine format
		RGB8,    // 3 8-bit components
		RGBA8,   // 4 8-bit components
		RGBA32F, // 4 32-bit float components
	};

	//-------------------------------------------------------------------------
	enum class Type : int {
		FLAT  = 0, // 2D texture
		CUBE  = 1, // 3D texture
		ARRAY = 2 // 2D texture array
	};

	//-------------------------------------------------------------------------  
	enum class Filter : int {
		LINEAR             = GL_LINEAR,
		NEAREST            = GL_NEAREST,
		NEAREST_MM_NEAREST = GL_NEAREST_MIPMAP_NEAREST,
		NEAREST_MM_LINEAR  = GL_NEAREST_MIPMAP_LINEAR,
		LINEAR_MM_NEAREST  = GL_LINEAR_MIPMAP_NEAREST,
		LINEAR_MM_LINEAR   = GL_LINEAR_MIPMAP_LINEAR
	};

	//-------------------------------------------------------------------------
	enum class Wrap : int {
		CLAMP,
		CLAMP_TO_EDGE,
		REPEAT
	};

	//-------------------------------------------------------------------------
private:
	uint32_t    m_texture; // texture handle

	std::string m_name;    // registered name

	Type        m_type; 
	Format      m_internal_format;

	int         m_dimensions[3]; // size of each dimension
	int         m_lod;           // number of level of depths

	int         Register();
	void        Unregister();

	GLenum      GetTarget() const;

	void        DefaultSettings( bool using_mipmap = false );
	static int  TranslateWrapModeGL( int mode );
	void        SaveDimensions( int width, int height, int depth = 1 );
	

public:
	/** -----------------------------------------------------------------------
	 * Use New().
	 */
	Texture( const Stref &name, Type type = Type::FLAT );

	virtual ~Texture();

	/** -----------------------------------------------------------------------
	 * Create a texture.
	 *
	 * @param format Pixel format.
	 * @param width  Width of texture.
	 * @param height Height of texture.
	 * @param depth  Depth, or number of slices in texture.
	 * @param lod    Number of levels of depth. (for mip mapping)
	 */
	void Create( Format format, int width, int height, 
		         int depth = 1, int lod=7 );

	/** -----------------------------------------------------------------------
	 * Load texture from a file.
	 *
	 * Supported formats are BMP, PNG, TGA, <other shit stb_image supports>
	 *
	 * For texture arrays, the height may be a multiple of the width.
	 * e.g. for a 16x16 image with 32 slices, 
	 * the image dimension should be 16x512
	 *
	 * @param filename Path to image file.
	 * @param format   Format to create texture, use Format::AUTO to copy the
	 *                 format of the source.
	 * @param lod      Number of levels to create the texture with.
	 *
	 * @returns false if an error occurs.
	 */
	bool LoadFile( const Stref &filename, Format format = Format::AUTO, 
		           int lod = 7 );

	/** -----------------------------------------------------------------------
	 * Load bitmap from memory.
	 *
	 * Create() must be used first.
	 *
	 * @param pixels Pointer to pixel data.
	 * @param level  What level to load this data into.
	 * @param x,y,z  Where in the texture to copy the pixel data to.
	 *
	 * @param width,height,depth Dimensions of the pixel data. 
	 *                           0 = use texture dimensions.
	 */
	void Load( const void *pixels, int level = 0, 
		       int x = 0, int y = 0, int z = 0, 
			   int width = 0, int height = 0, int depth = 0 );
	
	/** -----------------------------------------------------------------------
	 * Create an empty flat texture in F32 format (4xfloat)
	 *
	 * @param width,height Texture dimensions.
	 */
	void CreateEmpty_F32( int width, int height );

	/** -----------------------------------------------------------------------
	 * Create an empty texture array in RGBA format (4x8bit)
	 */
	void CreateEmptyArray_RGBA( int width, int height, int depth );
	
	/** -----------------------------------------------------------------------
	 * Update texture from memory.
	 *
	 * BGRA = 32bit (4x8bit) format
	 * BGR = 24bit (3x8bit) format
	 * RED = 8bit format
	 */
	void UpdateFromMemory_BGRA( const void *pixels, int width, int height);
	void UpdateFromMemory_BGR( const void *pixels, int width, int height );
	void UpdateFromMemory_RED( const void *pixels, int width, int height );
	
	/** -----------------------------------------------------------------------
	 * Modify a part of a texture array in RGBA format.
	 *
	 * @param x,y,z              Where to copy the data.
	 * @param width,height,depth Size of the data.
	 * @param pixels             Pointer to pixel data.
	 */
	void ArraySubImage_RGBA( int x, int y, int z, int width, 
		                     int height, int depth, const void *pixels );

	/** -----------------------------------------------------------------------
	 * Modify a part of a texture in F32 (float) format
	 *
	 * @param x,y,z              Where to copy the data.
	 * @param width,height,depth Size of the data.
	 * @param pixels             Pointer to pixel data.
	 */
	void SubImage_F32( int x, int y, int z, int width, 
		               int height, const void *pixels );
	
	/** -----------------------------------------------------------------------
	 * Generate mip maps.
	 *
	 * This function sets the texture filter to LINEAR_MM_LINEAR.
     */
	void AutoMipmap();
	
	/** -----------------------------------------------------------------------
	 * Set anisotropic sampling setting
	 *
	 * @param f 1 to GetMaxAnisotropy().
	 */
	void SetAnisotropy( float f );

	/** -----------------------------------------------------------------------
	 * Returns max anisotropy setting supported by the hardware.
	 */
	static float GetMaxAnisotropy();
	
	/** -----------------------------------------------------------------------
	 * Set texture filtering with simple settings
	 *
	 * @param filtered true for linear filtering, false for nearest filtering.
	 */
	void SetSimpleFilter( bool filtered );

	/** -----------------------------------------------------------------------
	 * Set texture filter settings.
	 *
	 * Use this instead of SetSimpleFilter for mipmap filters.
	 *
	 * @param mag    Filter to use when the texture is stretched (magnified)
	 * @param min    Filter to use when the texture is shrunken  (minified)
	 *
	 * @see Filter
	 */
	void SetFilter( Filter mag, Filter min );
	void SetFilter( Filter filter ) { SetFilter( filter, filter ); }
	
	/** -----------------------------------------------------------------------
	 * Set the texture wrapping mode.
	 *
	 * This controls how sampling works when near the edge of a texture.
	 * 
	 * @param horizontal_mode Option to use for horizontal wrapping.
	 * @param vertical_mode   Option to use for vertical wrapping.
	 */
	void SetWrapping( Wrap horizontal_mode, Wrap vertical_mode );
	void SetWrapping( Wrap mode ) { SetWrapping( mode, mode ); }
	
	/** -----------------------------------------------------------------------
	 * Bind this texture to the graphics layer.
	 */
	void Bind();

	/** -----------------------------------------------------------------------
	 * Get texture dimensions.
	 */
	int Width()  const { return m_dimensions[0]; }
	int Height() const { return m_dimensions[1]; }
	int Depth()  const { return m_dimensions[2]; }
	const int *Dimensions() const { return m_dimensions; }

	/** -----------------------------------------------------------------------
	 * Get the name used for this texture.
	 *
	 * May be empty for an unnamed texture.
	 */
	const std::string &Name() const { return m_name; }

	/** -----------------------------------------------------------------------
	 * Create a new texture.
	 *
	 * @param type Type of texture to create. See Type.
	 * @param name Name of texture, if supplied then a "named texture" will
	 *             be created and registered with the video instance.
	 */
	static ptr New( Type type = Type::FLAT, const Stref &name = "" );
};

}
