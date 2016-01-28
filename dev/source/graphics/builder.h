//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2015, Mukunda Johnson, All rights reserved. =========//

#pragma once

#include "graphics.h"
#include "vertexformats.h"

//-----------------------------------------------------------------------------
namespace Graphics {

/** ---------------------------------------------------------------------------
 * Handles buffering vertex data and loading it into elements with automatic
 * splitting of different material usages.
 */
class Builder {

private:

	int m_sort;

	int m_buffer_start;
	int m_buffer_size;

	Graphics::MaterialPtr      m_mat; 

	Video::VertexBuffer::Usage m_usage;
	Video::RenderMode          m_rendermode;
	RenderLayer                m_layer;
		
	ElementPtr                 m_element;

	VertexStream<Vertex::Texcola2D> m_vertex_data;

	Video::VertexBuffer::ptr m_vbo;

	/** -----------------------------------------------------------------------
	 * Output the current graphics element being built and start a new one.
	 *
	 * @param mat Material to use for the new element.
	 * @param sort sorting order for the new element.
	 */
	void Split( const Graphics::MaterialPtr &mat, int sort );
		
	/** -----------------------------------------------------------------------
	 * Output the current element being built and reset the pointer.
	 */
	void OutputElement();

public:

	Builder( Video::VertexBuffer::Usage usage, Video::RenderMode rendermode,
		     RenderLayer layer );
	virtual ~Builder();

	/** -----------------------------------------------------------------------
 	 * Start a new sub-element in this stream.
	 *
	 * @param mat Material to use for rendering the upcoming vertices.
	 * @param sort sorting order when rendering this object
	 */
	void New( const Graphics::MaterialPtr &mat, int sort = 0 );

	/** -----------------------------------------------------------------------
	 * Finish is called after all elements are done being loaded, to 
	 * output the final element being worked on and load the vertex buffer.
	 */
	void Finish();

	/** -----------------------------------------------------------------------
	 * Add a vertex to the current element.
	 */
	void AddVertex( float x, float y, float u, float v, 
		            uint8_t r, uint8_t g, uint8_t b, uint8_t a ) {

		m_vertex_data.Push( 
				Graphics::Vertex::Texcola2D( x, y, u, v, r, g, b, a ));

		m_buffer_size++;
	}
		
	/** -----------------------------------------------------------------------
	 * Add a white vertex to the current element.
	 */
	void AddVertex( float x, float y, float u, float v  ) {

		m_vertex_data.Push( 
				Graphics::Vertex::Texcola2D( x, y, u, v, 255,255,255,255 ));

		m_buffer_size++;
	}
};

}