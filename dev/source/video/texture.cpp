//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2015, Mukunda Johnson, All rights reserved. =========//

#include "stdafx.h"

#include "video/texture.h"
#include "util/minmax.h"
#include "video.h"
 
namespace Video {
	 
namespace {

	GLenum GL_FORMAT[] = { // Format -> GL format
		0,
		GL_RGB8,
		GL_RGBA8,
		GL_RGBA32F
	};

	

	GLenum INPUT_FORMAT[] = { // Format -> input format (create texture)
		0,
		GL_RGB,
		GL_RGBA,
		GL_RGBA
	};

	GLenum PIXEL_TYPE[] = { // format -> type (create texture)
		0,
		GL_UNSIGNED_BYTE,
		GL_UNSIGNED_BYTE,
		GL_FLOAT
	};
	
	// format conversions
	GLenum ToGLFormat( Texture::Format format ) {
		return GL_FORMAT[(int)format];
	}

	GLenum ToInputFormat( Texture::Format format ) {
		return INPUT_FORMAT[(int)format];
	}

	GLenum ToPixelType( Texture::Format format ) {
		return PIXEL_TYPE[(int)format];
	}
}
    
//-----------------------------------------------------------------------------
Texture::Texture( const Stref &name, Type type ) {
	glGenTextures( 1, &m_texture );
	m_type = type;
	m_lod = 1;
	
	for( int i = 0; i < 3; i++ ) {
		m_dimensions[i] = 0;
	}

	m_name = name;
}

//-----------------------------------------------------------------------------
Texture::~Texture() {

	glDeleteTextures( 1, &m_texture );
}

//-----------------------------------------------------------------------------
void Texture::Create( Format format, int width, int height, int depth, 
	                  int lod ) {

	assert( depth > 0 && width > 0 && height > 0 );
	assert( !(depth != 1 && m_type == Type::FLAT) );

	Bind();

	if( lod < 1 ) lod = 1;
	m_lod = lod;
	SaveDimensions( width, height, depth );

	m_internal_format = format;
	int target = GetTarget();
	int gl_format = GL_FORMAT[(int)format];
	if( m_type == Type::FLAT ) {
		
		for( int i = 0; i < m_lod; i++ ) {
			glTexImage2D( target, i, gl_format, width, height, 
				0, ToInputFormat(format), ToPixelType(format), NULL );

			width = Util::Max( 1, width >> 1 );
			height = Util::Max( 1, height >> 1 );
		}
		
	} else {
		
		for( int i = 0; i < m_lod; i++ ) {
			glTexImage3D( target, i, gl_format, width, height, depth, 
				0, ToInputFormat(format), ToPixelType(format), NULL );

			width = Util::Max( 1, width >> 1 );
			height = Util::Max( 1, height >> 1 );
			if( m_type == Type::CUBE ) {
				depth = Util::Max( 1, depth >> 1 );
			}
		}
	}

	if( m_lod == 1 ) {
		glTexParameteri( target, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		glTexParameteri( target, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	} else {
		glTexParameteri( target, GL_TEXTURE_MIN_FILTER, 
			                     GL_LINEAR_MIPMAP_LINEAR );
		glTexParameteri( target, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	}
}

//-----------------------------------------------------------------------------
GLenum Texture::GetTarget() const {

	static const int targets[] = {
		GL_TEXTURE_2D,
		GL_TEXTURE_3D,
		GL_TEXTURE_2D_ARRAY
	};

	return targets[ (int)m_type ];
}

//-----------------------------------------------------------------------------
void Texture::SaveDimensions( int width, int height, int depth ) {
	m_dimensions[0] = width;
	m_dimensions[1] = height;
	m_dimensions[2] = depth;
}
 
//-----------------------------------------------------------------------------
bool Texture::LoadFile( const Stref &filename, Format format, int lod ) {
	StbImage img( *filename );
	if( !img.Valid() ) return false;
	Bind();
	if( lod < 1 ) lod = 1;

	GLenum input_format = img.Components() == 3 ? GL_RGB : GL_RGBA;
	if( format == Format::AUTO ) {
		format = img.Components() == 3 ? Format::RGB8 : Format::RGBA8;
	}
	
	if( img.Width() == img.Height() ) {
		if( m_type != Type::FLAT ) {
			return false;
		}
		Create( format, img.Width(), img.Height(), 1, lod );

		glTexSubImage2D( GetTarget(), 0, 0, 0, img.Width(), img.Height(),
			             input_format, GL_UNSIGNED_BYTE, img.Data() );

	} else {
		if( img.Height() % img.Width() != 0 ) {
			return false;
		}
		if( m_type != Type::ARRAY ) {
			return false;
		}

		Create( format, img.Width(), img.Width(), 
			    img.Height() / img.Width(), lod );

		glTexSubImage3D( GetTarget(), 0, 0, 0, 0, img.Width(), img.Width(), 
			             img.Height() / img.Width(), input_format, 
			             GL_UNSIGNED_BYTE, img.Data() );

		
	}

	// generate mipmaps?
	if( lod != 1 ) {
		AutoMipmap();
	}

	return true;
}
  
//-----------------------------------------------------------------------------
void Texture::Load( const void *pixels, int level, int x, int y, int z, 
	                int width, int height, int depth ) {
	Bind();
	
	if( width == 0 ) width   = m_dimensions[0];
	if( height == 0 ) height = m_dimensions[1];
	if( depth == 0 ) depth   = m_dimensions[2];

	if( m_type == Type::FLAT ) {

		glTexSubImage2D( GetTarget(), level, x, y, width, height, 
						 ToInputFormat(m_internal_format), 
						 ToPixelType(m_internal_format), pixels );

	} else {
		glTexSubImage3D( GetTarget(), level, x, y, z, width, height, depth, 
						 ToInputFormat(m_internal_format), 
						 ToPixelType(m_internal_format), pixels );
	}
}

//-----------------------------------------------------------------------------
void Texture::AutoMipmap() {

	Bind();
	int target = GetTarget();
	glTexParameteri( target, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri( target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
	glGenerateMipmap( target );
}
	
//-----------------------------------------------------------------------------
void Texture::SetAnisotropy( float f ) {

	Bind();
	int target = GetTarget();
	glTexParameterf( target, GL_TEXTURE_MAX_ANISOTROPY_EXT, f );
}

//-----------------------------------------------------------------------------
float Texture::GetMaxAnisotropy() {
	float max_anisotropy;
	glGetFloatv( GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &max_anisotropy );
	return max_anisotropy;
}

//-----------------------------------------------------------------------------
void Texture::SetSimpleFilter( bool filtered ) {

	if( filtered ) {
		SetFilter( Filter::LINEAR );
	} else {
		SetFilter( Filter::NEAREST );
	}
}

//-----------------------------------------------------------------------------
void Texture::SetFilter( Filter mag, Filter min ) {

	int target = GetTarget();
	Bind();
	glTexParameteri( target, GL_TEXTURE_MAG_FILTER, (int)mag );
	glTexParameteri( target, GL_TEXTURE_MIN_FILTER, (int)min );
}

//-----------------------------------------------------------------------------
int Texture::TranslateWrapModeGL( int mode )  {

	static const int mode_table[] = {
		GL_CLAMP,
		GL_CLAMP_TO_EDGE,
		GL_REPEAT
	};
	return mode_table[mode];
}

//-----------------------------------------------------------------------------
void Texture::SetWrapping( Wrap horizontal_mode, Wrap vertical_mode ) {
	int target = GetTarget();
	Bind();

	int gl_h, gl_v;
	gl_h = TranslateWrapModeGL( (int)horizontal_mode );
	gl_v = TranslateWrapModeGL( (int)vertical_mode );

	glTexParameteri( target, GL_TEXTURE_WRAP_S, gl_h );
	glTexParameteri( target, GL_TEXTURE_WRAP_T, gl_v );
}
  
//-----------------------------------------------------------------------------
void Texture::Bind() {
	glBindTexture( GetTarget(), m_texture );
}
 
//-----------------------------------------------------------------------------
Texture::ptr Texture::New( Type type, const Stref &name ) {
	auto tex = std::make_shared<Texture>( name, type );

	if( tex->m_name != "" ) {
		Video::RegisterTexture( tex ); 
	}
	return tex;
}

} // namespace Video

