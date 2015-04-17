//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//

#include "stdafx.h"
#include "video/vertexbuffer.h"
#include "video/shader.h"

//-------------------------------------------------------------------------------------------------
namespace Video {
	
//-------------------------------------------------------------------------------------------------
VertexBuffer::VertexBuffer( GLenum usage, int p_format, GLuint nbuffers ) {
	assert( nbuffers > 0 );
	m_buffer_count = nbuffers; 
	m_format = p_format;
	m_usage = usage;

	m_ids = new GLuint[m_buffer_count];
	glGenBuffers( m_buffer_count, m_ids ); // TODO ERROR CHECKING!
}

//-------------------------------------------------------------------------------------------------
VertexBuffer::~VertexBuffer() {
	if( m_buffer_count != 0 ) {
		glDeleteBuffers( m_buffer_count, m_ids ); // TODO: ERROR CHECKING (?)
		delete[]m_ids;
		m_buffer_count = 0;
	}
}
 
//-------------------------------------------------------------------------------------------------
void VertexBuffer::Bind( GLuint index ) {
	assert( index >= 0 && index < m_buffer_count );
	Video::BindArrayBuffer( m_ids[index] ); 
}

//-------------------------------------------------------------------------------------------------
void VertexBuffer::Load( void *data, GLsizei size, GLuint index ) {
	Bind(index);
	glBufferData( GL_ARRAY_BUFFER, size, data, m_usage );
}

//-------------------------------------------------------------------------------------------------
void VertexBuffer::Render( GLenum mode, GLuint offset, GLuint start, GLuint size, GLuint index ) {
	Bind(index);

	Video::Shader *s = Video::GetActiveShader();
	if( !s ) return;

	s->SetVertexAttributePointers( offset );
	glDrawArrays( mode, start, size );
}

//-------------------------------------------------------------------------------------------------
}
