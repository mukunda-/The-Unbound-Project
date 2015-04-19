//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2015, Mukunda Johnson, All rights reserved. =========//
 
#pragma once

#include "video/video.h"
#include "mem/memorylib.h"

//-----------------------------------------------------------------------------
namespace Video {

//-----------------------------------------------------------------------------
class VertexBuffer : public Memory::FastAllocation {	

public:
	
	//-------------------------------------------------------------------------
	enum class Usage {

		// Usage hints

		// STATIC means the data in VBO will not be changed (specified once 
		// and used many times), DYNAMIC means the data will be changed 
		// frequently (specified and used repeatedly), and STREAM means the 
		// data will be changed every frame (specified once and used once). 
		
		// DRAW means the data will be sent to GPU in order to draw 
		// (application to GL), READ means the data will be read by the 
		// client's application (GL to application), and COPY means the data 
		// will be used both drawing and reading (GL to GL).

		STREAM_DRAW  = GL_STREAM_DRAW,
		STREAM_READ  = GL_STREAM_READ,
		STREAM_COPY  = GL_STREAM_COPY,
		STATIC_DRAW  = GL_STATIC_DRAW,
		STATIC_READ  = GL_STATIC_READ,
		STATIC_COPY  = GL_STATIC_COPY,
		DYNAMIC_DRAW = GL_DYNAMIC_DRAW,
		DYNAMIC_READ = GL_DYNAMIC_READ,
		DYNAMIC_COPY = GL_DYNAMIC_COPY
	};

private:
	std::unique_ptr<GLuint[]> m_ids; // list of buffer IDs, 
	                                 // usually just a single id.
	int    m_buffer_count;	// number of buffers allocated from gl
	int    m_format;		// unused for now

	Usage  m_usage;			// usage hint for rendering 
	 
	VertexBuffer( Usage usage, int format, int buffers );

	//-------------------------------------------------------------------------
public:
	typedef std::shared_ptr<VertexBuffer> ptr;

	virtual ~VertexBuffer();

	/** -----------------------------------------------------------------------
	 * Bind this buffer.
	 *
	 * @param index Index of buffer to bind, if multiple buffers were
	 *        allocated.
	 */
	void Bind( int index );

	/** -----------------------------------------------------------------------
	 * Get the OpenGL buffer handle.
	 */
	GLuint GetHandle( int index );
	
	/** -----------------------------------------------------------------------
	 * Load data into the buffer.
	 *
	 * @param data  Vertex data pointer.
	 * @param size  Size of data in bytes.
	 * @param index Index of VBO to use
	 */
	void Load( void *data, int size, int index = 0 );

	/** -----------------------------------------------------------------------
	 * Do a primitive render call.
	 *
	 * @param mode Rendering mode.
	 * @param offset Offset into the vertex data in bytes.
	 * @param start  Vertex index to start on.
	 * @param size   Number of vertices to render.
	 * @param index  Index of buffer to draw from.
	 */
	void Render( Video::RenderMode mode, int offset, int start, int size, 
		         int index = 0 );

	
	/** -----------------------------------------------------------------------
	 * Create a vertex buffer.
	 *
	 * @param usage   Usage hint.
	 * @param format  Vertex format, currently not used.
	 * @param buffers Number of vertex buffer objects to allocate.
	 */
	static ptr Create( Usage usage = Usage::STATIC_DRAW, int format = 0, 
		               int buffers = 1 ) {

		return std::make_shared<VertexBuffer>( usage, format, buffers );
	}
};

}
