//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2015, Mukunda Johnson, All rights reserved. =========//

#include "stdafx.h"
#include "video/vertexbuffer.h"
#include "video/shader.h"

//-----------------------------------------------------------------------------
namespace Video {
	
//-----------------------------------------------------------------------------
VertexBuffer::VertexBuffer( Usage usage, int format, int buffers ) {
	assert( buffers > 0 );
	m_buffer_count = buffers; 
	m_format = format;
	m_usage = usage;

	m_ids.reset( new GLuint[m_buffer_count] );
	glGenBuffers( m_buffer_count, m_ids.get() ); // TODO ERROR CHECKING!
}

//-----------------------------------------------------------------------------
VertexBuffer::~VertexBuffer() {
	if( m_buffer_count != 0 ) {

		// TODO: ERROR CHECKING (?)
		glDeleteBuffers( m_buffer_count, m_ids.get() ); 
		m_ids.reset();
		m_buffer_count = 0;
	}
}
 
//-----------------------------------------------------------------------------
void VertexBuffer::Bind( int index ) {
	assert( index >= 0 && index < m_buffer_count );
	Video::BindArrayBuffer( m_ids[index] ); 
}

//-----------------------------------------------------------------------------
void VertexBuffer::Load( void *data, int size, int index ) {
	Bind(index);
	glBufferData( GL_ARRAY_BUFFER, size, data, (GLenum)m_usage );
}

//-----------------------------------------------------------------------------
void VertexBuffer::Render( Video::RenderMode mode, int offset, int start, 
	                       int size, int index ) {
	Bind(index);

	Video::Shader *s = Video::GetActiveShader();
	if( !s ) return;

	s->SetVertexAttributePointers( offset );
	glDrawArrays( (GLenum)mode, start, size );
}

//-----------------------------------------------------------------------------
}
