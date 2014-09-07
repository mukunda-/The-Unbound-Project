//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//

#pragma once

#include "util/trie.h"

namespace Video {

class Texture : public std::enable_shared_from_this<Texture> {

public:
	enum Format {
		FORMAT_AUTO,
		FORMAT_RGB8,
		FORMAT_RGBA8,
		FORMAT_RGBA32F,

	};

	enum Type {
		TEXTURE_2D,
		TEXTURE_3D,
		TEXTURE_2D_ARRAY
	};

	
	//-----------------------------------------------------------------------------------
	// texture filter modes
	//
	enum {
		FILTER_LINEAR = GL_LINEAR,
		FILTER_NEAREST = GL_NEAREST,
		FILTER_NEAREST_MM_NEAREST = GL_NEAREST_MIPMAP_NEAREST,
		FILTER_NEAREST_MM_LINEAR = GL_NEAREST_MIPMAP_LINEAR,
		FILTER_LINEAR_MM_NEAREST = GL_LINEAR_MIPMAP_NEAREST,
		FILTER_LINEAR_MM_LINEAR = GL_LINEAR_MIPMAP_LINEAR
	};

	//-----------------------------------------------------------------------------------
	// texture wrap modes
	//
	enum {
		WRAP_CLAMP,
		WRAP_CLAMP_TO_EDGE,
		WRAP_REPEAT
	};

private:
	GLuint m_texture;

	std::string m_name;

	Type m_type; 

	Format m_internal_format;

	int m_dimensions[3];
	int m_lod;

	int Register();
	void Unregister();

	GLenum GetTarget() const;

	void DefaultSettings( bool using_mipmap = false );
	static int TranslateWrapModeGL( int mode );
	void SaveDimensions( int width, int height, int depth = 1 );
	
	Texture( const char *name = nullptr, Type type = TEXTURE_2D );

public:
	~Texture();

	//-----------------------------------------------------------------------------------
	// create a texture
	//
	void Create( Format format, int width, int height, int depth=1, int lod=7 );

	//-----------------------------------------------------------------------------------
	// load texture from a file
	// supported formats are BMP, PNG, TGA, <other shit stb_image supports>
	//
	// for texture arrays, the height may be a multiple of the width
	// number of slices will be height/width
	//
	bool LoadFile( const char *filename, Format format=FORMAT_AUTO, int lod=7 );

	//-----------------------------------------------------------------------------------
	// load image from memory
	//
	void Load( const void *pixels, int level=0, int x=0, int y=0, int z=0, int width=0, int height=0, int depth=0 );
	
	//-----------------------------------------------------------------------------------
	// load texture from a font
	//
	//void LoadFont( RasterFonts::Font *font );

	//-----------------------------------------------------------------------------------
	// create empty texture in F32 format (4xfloat)
	//
	void CreateEmpty_F32( int p_width, int p_height );

	//-----------------------------------------------------------------------------------
	// create empty texture array in RGBA format (4x8bit)
	//
	void CreateEmptyArray_RGBA( int p_width, int p_height, int p_depth );
	
	//-----------------------------------------------------------------------------------
	// update texture from memory
	//
	// BGRA = 32bit (4x8bit) format
	// BGR = 24bit (3x8bit) format
	// RED = 8bit format
	//
	void UpdateFromMemory_BGRA( const void *pixels, int p_width, int p_height );
	void UpdateFromMemory_BGR( const void *pixels, int p_width, int p_height );
	void UpdateFromMemory_RED( const void *pixels, int p_width, int p_height );
	
	//-----------------------------------------------------------------------------------
	// modify a part of a texture array
	// format = RGBA
	//
	void ArraySubImage_RGBA( int x, int y, int z, int p_width, int p_height, int p_depth, const void *pixels );

	//-----------------------------------------------------------------------------------
	// modify a part of a texture in F32 (float) format
	//
	void SubImage_F32( int x, int y, int z, int p_width, int p_height, const void *pixels );
	
	//-----------------------------------------------------------------------------------
	// generate mipmap thingy
	//
	// this function sets the texture filter to linear+mipmaps
	//
	void AutoMipmap();
	
	//-----------------------------------------------------------------------------------
	// set anisotropic sampling setting
	//
	void SetAnisotropy( float f );

	//-----------------------------------------------------------------------------------
	// get max anisotropy setting supported by the hardware
	//
	static float GetMaxAnisotropy();
		
	//-----------------------------------------------------------------------------------
	// set texture filtering with simple settings
	// filtered = linear filtering
	// !filtered = nearest filtering
	//
	void SetSimpleFilter( bool filtered );

	//-----------------------------------------------------------------------------------
	// set texture filter settings
	// mag = filter to use when the texture is stretched (magnified)
	// min = filter to use when the texture is shrunken (minified)
	// see FILTER_* enum
	//
	// use this instead of simple for mipmap usage
	//
	void SetFilter( int mag, int min );

	//-----------------------------------------------------------------------------------
	// set the texture wrapping mode
	// seee WRAP_* enum
	//
	void SetWrapping( int mode );
	void SetWrapping( int horizontal_mode, int vertical_mode );
	
	//-----------------------------------------------------------------------------------
	// bind this texture for modification or rendering
	//
	void Bind();

	//-----------------------------------------------------------------------------------
	// read dimensions
	//
	int Width() const;
	int Height() const;
	int Depth() const;

	const std::string &Name() const;

	typedef std::shared_ptr<Texture> Pointer;

	static Pointer New( const char *name = nullptr, Type type = TEXTURE_2D );
};
 
bool Find( const char *name, Texture::Pointer &result );

}
