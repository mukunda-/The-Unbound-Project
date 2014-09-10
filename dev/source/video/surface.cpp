//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//

#include "stdafx.h"

#if 0 // TODO

//-------------------------------------------------------------------------------------------------
namespace Video {
	
//-------------------------------------------------------------------------------------------------
extern int screen_width, screen_height;

//-------------------------------------------------------------------------------------------------
Surface::Surface() {
	has_depth = false;
	filtered = false;
	created = false;
	ratio = 0;
	rendering_buffer = 0;
	frame_buffer_texture = 0;
	frame_buffer_object = 0;
}

//-------------------------------------------------------------------------------------------------
void Surface::SetFiltering( bool p_filtered ) {
	filtered = p_filtered;
	glBindTexture(GL_TEXTURE_2D, frame_buffer_texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filtered ? GL_LINEAR : GL_NEAREST );
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filtered ? GL_LINEAR : GL_NEAREST );
}

//-------------------------------------------------------------------------------------------------
bool Surface::CreateI( bool p_filtered, bool p_depth, int p_width, int p_height ) {
	assert( !created ); // this function shouldn't be called if the surface is already created
	
	width = p_width;
	height = p_height;
	
	glGenTextures( 1, &frame_buffer_texture );

	SetFiltering( p_filtered );
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	
	glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE); // automatic mipmap
	// (mipmap for render surface???)

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, p_width, p_height, 0, GL_RGBA, GL_BYTE, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	
	
	/*
	if( depth ) { 
		glGenRenderbuffersEXT(1, &rendering_buffer );
		glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, rendering_buffer);
		glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT, p_width, p_height);
		glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, 0);
	} else {
		rendering_buffer=0;
	}*/
	
	// create a framebuffer object
	
	glGenFramebuffersEXT( 1, &frame_buffer_object );
	glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, frame_buffer_object );
	// attach the texture to FBO color attachment point
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT,
							  GL_TEXTURE_2D, frame_buffer_texture, 0);
	// attach the renderbuffer to depth attachment point

	
	if( p_depth ) {
		AddDepthBuffer();
	}

	return true;
}

//-------------------------------------------------------------------------------------------------
bool Surface::Create( bool p_filtered, bool p_depth, int p_width, int p_height ) {
	ratio = 0;
	return CreateI( p_filtered, p_depth, p_width, p_height );
}

//-------------------------------------------------------------------------------------------------
bool Surface::Create( bool p_filtered, bool p_depth, int p_ratio ) {
	ratio = p_ratio;
	if( ratio <= 0 ) return false;
	return CreateI( p_filtered, p_depth, screen_width / ratio, screen_height / ratio );
}

//-------------------------------------------------------------------------------------------------
void Surface::Destroy() {
	if( created ) {
		// todo lol
	}

}

//-------------------------------------------------------------------------------------------------
void Surface::Use() {
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, frame_buffer_object );
	glViewport( 0, 0, width, height );
}

//-------------------------------------------------------------------------------------------------
void Surface::BindTexture(  ) {
	glBindTexture( GL_TEXTURE_2D, frame_buffer_texture );
}

//-------------------------------------------------------------------------------------------------
void Surface::AddDepthBuffer() {
	assert( !has_depth );
	has_depth = true;
	// todo: error checking
	glGenRenderbuffersEXT(1, &rendering_buffer );
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, rendering_buffer);
	glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT, width, height);
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, 0);

	glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, frame_buffer_object );
	glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, rendering_buffer );
}

//-------------------------------------------------------------------------------------------------
void Surface::LinkDepthBuffer( Surface *p ) {
	if( p->GetDepthBuffer() ) {
		glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, frame_buffer_object );
		glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, p->GetDepthBuffer() );
	}
}

//-------------------------------------------------------------------------------------------------
GLuint Surface::GetDepthBuffer() {
	return rendering_buffer;
}

//-------------------------------------------------------------------------------------------------
}
#endif
