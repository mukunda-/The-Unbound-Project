//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//

#pragma once

#include "video/video.h"
#include "mem/memorylib.h"

//-------------------------------------------------------------------------------------------------
namespace Video {

//-------------------------------------------------------------------------------------------------
class VertexBuffer : public Memory::FastAllocation {	
	GLuint *m_ids;			// list of buffer IDs, usually just a single id
	GLuint m_buffer_count;	// number of buffers allocated from gl
	int m_format;			// unused for now

	GLenum m_usage;			// usage hint for rendering 
	 
public:
	typedef std::shared_ptr<VertexBuffer> Pointer;

	// create a vertex buffer
	// usage : opengl usage hint
	// p_format : unused
	// buffers : number of vertex buffer objects to allocate
	//
	VertexBuffer( GLenum usage = GL_STATIC_DRAW, int p_format = 0, GLuint buffers = 1 );
	~VertexBuffer();

	// bind a vertex buffer
	//
	void Bind( GLuint index = 0 );

	// get the GL handle
	//
	GLuint GetHandle( GLuint index = 0 );

	// load vertex data into the buffer
	// data : vertex data pointer
	// size : size of vertex data in bytes
	// index: index of VBO to use
	//
	void Load( void *data, GLsizei size, GLuint index = 0 );

	// do a primitive render call
	// mode   : gl rendering mode
	// offset : offset into vertex data in bytes
	// start  : vertex index to start on
	// size   : number of vertices to render
	// index: index of VBO to use
	//
	void Render( GLenum mode, GLuint offset, GLuint start, GLuint size, GLuint index = 0 );

	// wrapper to create a shared vertex buffer pointer
	//
	static Pointer Create( GLenum usage = GL_STATIC_DRAW, int p_format = 0, GLuint buffers = 1 ) {
		return Pointer( new VertexBuffer( usage, p_format, buffers ) );
	}
};

}
