//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//

#include "stdafx.h"
#include "video/textures.h"
#include "util/minmax.h"

/*

texture table

32-bit ID
tree to map IDs to loaded indexes

1024 TEXTURE INDEXES IN WORLD ATLAS
 
*/

namespace Video {

	/*
// a class that maps a 32-bit number to data
template <class Data> class Map32 {
	
	// AA BB CC DD

	struct Branch : public Memory::FastAllocation {
		void *data[0x100];
		int references;
		Branch *parent;

		Branch( Branch *p_parent ) {
			memset( data, 0, sizeof data );
			counter = 0;
			parent = p_parent;
		}
	};

	struct Leaf : public Memory::FastAllocation {
		Data *data[0x100]; // index into texture space
		int references;

		Leaf( Branch *p_parent ) {
			memset( data, 0, sizeof data );
		}
	};

	Branch trunk;

	void DeleteLeaf( Leaf *leaf ) {
		
	}

public:
	Data *Get( boost::uint32_t index ) {
		Branch *b = trunk.data[index>>24];
		if( !b ) return 0;
		b = b->data[(index>>16)&0xFF];
		if( !b ) return 0;
		b = b->data[(index>>8)&0xFF];
		if( !b ) return 0;
		return b[index&0xFF];
	}

	void Set( boost::uint32_t index, Data *value ) {
		Branch *t = &trunk;
		Branch *b;
		b = static_cast<Branch*>t->data[index>>24];
		if( !b ) t->data[index>>24] = b = new Branch;
		t = b;
		b = static_cast<Branch*>t->data[(index>>16)&0xFF];
		if( !b ) t->data[(index>>16)&0xFF] = b = new Branch;
		t = b;
		Leaf *leaf = static_cast<Leaf*>t->data[(index>>8)&0xFF];
		if( !leaf ) t->data[(index>>8)&0xFF] = leaf = new Leaf;

		if( b[index&0xFF] == 0 && value != 0 ) {
			b->references++;
		} else if( b[index&0xFF] != 0 && value == 0 ) {
			b->references--;
			if( b->references == 0 ) {
				DeleteLeaf( b );
			}
		}
		b[index&0xFF] = value;
	}

};
*/

Util::Trie<Texture::Pointer> texture_table;	

// format conversions
GLenum GL_FORMAT[] = { // format -> internal format
	0,
	GL_RGB8,
	GL_RGBA8,
	GL_RGBA32F
};

GLenum INPUT_FORMAT[] = { // format -> input format
	0,
	GL_RGB,
	GL_RGBA,
	GL_RGBA
};

GLenum PIXEL_TYPE[] = { // format -> type
	0,
	GL_UNSIGNED_BYTE,
	GL_UNSIGNED_BYTE,
	GL_FLOAT
};

//-------------------------------------------------------------------------------------------------
static void RegisterTexture( Texture::Pointer &texture ) {
	assert( texture->Name() != "" );
	if( !texture_table.Set( texture->Name().c_str(), texture, false ) ) {
		throw std::runtime_error( "Texture name collision." );
	}
}

//-------------------------------------------------------------------------------------------------
static void UnregisterTexture( const std::string &name ) {
	assert( name != "" );
	texture_table.Reset( name.c_str(), nullptr );
}

//-------------------------------------------------------------------------------------------------
bool Find( const char *name, Texture::Pointer &result ) {
	if( !texture_table.Get( name, result ) ) return false;
	return true;
}
           
//-------------------------------------------------------------------------------------------------
Texture::Texture( const char *name, Type type ) {
	glGenTextures( 1, &m_texture );
	m_type = type;
	m_lod = 1;
	for( int i = 0; i < 3; i++ ) m_dimensions[i] = 0;

	if( name ) m_name = name;
}

//-------------------------------------------------------------------------------------------------
Texture::~Texture() {
	glDeleteTextures( 1, &m_texture );
	if( m_name != "" ) {
		UnregisterTexture( m_name );
	}
}

//-------------------------------------------------------------------------------------------------
void Texture::Create( Format format, int width, int height, int depth, int lod ) {
	assert( depth > 0 && width > 0 && height > 0 );
	assert( !(depth != 1 && m_type == TEXTURE_2D) );

	Bind();

	if( lod < 1 ) lod = 1;
	m_lod = lod;
	SaveDimensions( width, height, depth );

	m_internal_format = format;
	int target = GetTarget();
	int gl_format = GL_FORMAT[format];
	if( m_type == TEXTURE_2D ) {
		
		for( int i = 0; i < m_lod; i++ ) {
			glTexImage2D( target, i, gl_format, width, height, 
				0, INPUT_FORMAT[format], PIXEL_TYPE[format], NULL );

			width = Util::Max( 1, width >> 1 );
			height = Util::Max( 1, height >> 1 );
		}
		
	} else {
		
		for( int i = 0; i < m_lod; i++ ) {
			glTexImage3D( target, i, gl_format, width, height, depth, 
				0, INPUT_FORMAT[format], PIXEL_TYPE[format], NULL );

			width = Util::Max( 1, width >> 1 );
			height = Util::Max( 1, height >> 1 );
			if( m_type == TEXTURE_3D ) {
				depth = Util::Max( 1, depth >> 1 );
			}
		}
	}

	if( m_lod == 1 ) {
		glTexParameteri( target, GL_TEXTURE_MIN_FILTER,GL_LINEAR );
		glTexParameteri( target, GL_TEXTURE_MAG_FILTER,GL_LINEAR );
	} else {
		glTexParameteri( target, GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR );
		glTexParameteri( target, GL_TEXTURE_MAG_FILTER,GL_LINEAR );
	}
}

//-------------------------------------------------------------------------------------------------
GLenum Texture::GetTarget() const {

	static const int targets[] = {
		GL_TEXTURE_2D,
		GL_TEXTURE_3D,
		GL_TEXTURE_2D_ARRAY
	};

	return targets[m_type];
}

//-------------------------------------------------------------------------------------------------
void Texture::SaveDimensions( int width, int height, int depth ) {
	m_dimensions[0] = width;
	m_dimensions[1] = height;
	m_dimensions[2] = depth;
}

//-------------------------------------------------------------------------------------------------
int Texture::Width() const {
	return m_dimensions[0];
}

//-------------------------------------------------------------------------------------------------
int Texture::Height() const {
	return m_dimensions[1];
}

//-------------------------------------------------------------------------------------------------
int Texture::Depth() const {
	return m_dimensions[2];
}

const std::string &Texture::Name() const {
	return m_name;
}

//-------------------------------------------------------------------------------------------------
bool Texture::LoadFile( const char *filename, Format format, int lod ) {
	StbImage img( filename );
	if( !img.Valid() ) return false;
	Bind();
	if( lod < 1 ) lod = 1;

	GLenum input_format = img.Components() == 3 ? GL_RGB : GL_RGBA;
	if( format == FORMAT_AUTO ) {
		format = img.Components() == 3 ? FORMAT_RGB8 : FORMAT_RGBA8;
	}
	
	if( img.Width() == img.Height() ) {
		if( m_type != TEXTURE_2D ) {
			return false;
		}
		Create( format, img.Width(), img.Height(), 1, lod );
		glTexSubImage2D( GetTarget(), 0, 0, 0, img.Width(), img.Height(),
			input_format, GL_UNSIGNED_BYTE, img.Data() );

	} else {
		if( img.Height() % img.Width() != 0 ) {
			return false;
		}
		if( m_type != TEXTURE_2D_ARRAY ) {
			return false;
		}
		Create( format, img.Width(), img.Width(), img.Height() / img.Width(), lod );
		glTexSubImage3D( GetTarget(), 0, 0, 0, 0, img.Width(), img.Width(), img.Height() / img.Width(), 
			input_format, GL_UNSIGNED_BYTE, img.Data() );

		
	}
	// generate mipmaps?
	if( lod != 1 ) {
		AutoMipmap();
	}
	return true;
}
  
//-------------------------------------------------------------------------------------------------
void Texture::Load( const void *pixels, int level, int x, int y, int z, int width, int height, int depth ) {
	
	Bind();
	
	if( width == 0 ) width   = m_dimensions[0];
	if( height == 0 ) height = m_dimensions[1];
	if( depth == 0 ) depth   = m_dimensions[2];

	if( m_type == TEXTURE_2D ) {
		glTexSubImage2D( GetTarget(), level, x, y, width, height, 
			INPUT_FORMAT[m_internal_format], PIXEL_TYPE[m_internal_format], pixels );
	} else {
		glTexSubImage3D( GetTarget(), level, x, y, z, width, height, depth, 
			INPUT_FORMAT[m_internal_format], PIXEL_TYPE[m_internal_format], pixels );
	}
}
 
	
//-------------------------------------------------------------------------------------------------
void Texture::AutoMipmap() {
	Bind();
	int target = GetTarget();
	glTexParameteri( target, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri( target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
	glGenerateMipmap( target );

}
	
//-------------------------------------------------------------------------------------------------
void Texture::SetAnisotropy( float f ) {
	Bind();
	int target = GetTarget();
	glTexParameterf( target, GL_TEXTURE_MAX_ANISOTROPY_EXT, f );
}

//-------------------------------------------------------------------------------------------------
float Texture::GetMaxAnisotropy() {
	float max_anisotropy;
	glGetFloatv( GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &max_anisotropy );
	return max_anisotropy;
}

//-------------------------------------------------------------------------------------------------
void Texture::SetSimpleFilter( bool filtered ) {
	int target = GetTarget();
	Bind();
	glTexParameteri( target, GL_TEXTURE_MAG_FILTER, filtered ? GL_LINEAR : GL_NEAREST );
	glTexParameteri( target, GL_TEXTURE_MIN_FILTER, filtered ? GL_LINEAR : GL_NEAREST );
}

//-------------------------------------------------------------------------------------------------
void Texture::SetFilter( int mag, int min ) {
	int target = GetTarget();
	Bind();
	glTexParameteri( target, GL_TEXTURE_MAG_FILTER, mag );
	glTexParameteri( target, GL_TEXTURE_MIN_FILTER, min );
}

//-------------------------------------------------------------------------------------------------
int Texture::TranslateWrapModeGL( int mode )  {
	static const int mode_table[] = {
		GL_CLAMP,
		GL_CLAMP_TO_EDGE,
		//GL_WRAP //??? todo!
	};
	return mode_table[mode];
}

//-------------------------------------------------------------------------------------------------
void Texture::SetWrapping( int horizontal_mode, int vertical_mode ) {
	int target = GetTarget();
	Bind();

	int gl_h, gl_v;
	gl_h = TranslateWrapModeGL( horizontal_mode );
	gl_v = TranslateWrapModeGL( vertical_mode );

	glTexParameteri( target, GL_TEXTURE_WRAP_S, gl_h );
	glTexParameteri( target, GL_TEXTURE_WRAP_T, gl_v );
}

//-------------------------------------------------------------------------------------------------
void Texture::SetWrapping( int mode ) {
	SetWrapping( mode, mode );
}

//-------------------------------------------------------------------------------------------------
void Texture::Bind() {
	glBindTexture( GetTarget(), m_texture );
}
 
 
Texture::Pointer Texture::New( const char *name, Type type ) {
	Pointer tex( new Texture( name, type ) );
	if( tex->m_name != "" ) {
		RegisterTexture( tex );
	}
	return tex;
}

} // namespace Video

