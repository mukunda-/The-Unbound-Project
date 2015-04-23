//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2015, Mukunda Johnson, All rights reserved. =========//

#include "stdafx.h"
#include "builder.h"

//-----------------------------------------------------------------------------
namespace Graphics {

//-----------------------------------------------------------------------------
Builder::Builder( Video::VertexBuffer::Usage usage, 
				  Video::RenderMode rendermode, 
				  RenderLayer layer ) {

	m_element      = nullptr;
	m_usage        = usage;
	m_rendermode   = rendermode;
	m_layer        = layer;
	m_buffer_start = 0;
	m_buffer_size  = 0;

	m_vbo = Video::VertexBuffer::Create( m_usage );
}

//-----------------------------------------------------------------------------
Builder::~Builder() {}

//-----------------------------------------------------------------------------
void Builder::New( const Graphics::MaterialPtr &mat, int sort ) {

	// New is called before adding more vertices
	// it splits the element if the material or sorting dont match
	//
	if( !m_element || m_element->GetMaterial() != mat  
		    || m_element->GetSort() != sort ) {

		Split( mat, sort );
	}
}

//-----------------------------------------------------------------------------
void Builder::Finish() {
	// end is called after everything is added, to add the final element
	// and load the vertex buffer
	OutputElement();

	m_vertex_data.Load( *m_vbo );

	m_buffer_start = 0;
	m_buffer_size  = 0;
}

//-----------------------------------------------------------------------------
void Builder::Split( const Graphics::MaterialPtr &mat, int sort ) {
	OutputElement();
			
	m_mat         = mat;
	m_sort        = sort;
	m_buffer_size = 0;

	// setup default Split settings
	m_element = Graphics::CreateElement();
	m_element->Setup( m_vbo, Video::BlendMode::ALPHA, 
				        m_mat, 0, m_rendermode );

	m_element->SetPostAction( Graphics::Element::PostAction::REMOVE ); 
	m_element->SetBufferParams( 0, 0, m_buffer_start, 0 );
	m_element->SetSort( sort );
	
	m_element->SetRenderLayer( m_layer );
}

//-----------------------------------------------------------------------------
void Builder::OutputElement() {
	if( m_element ) {
		if( m_buffer_size != 0 ) {
			m_element->SetBufferParams( -1, -1, -1, m_buffer_size );
			m_buffer_start += m_buffer_size;
			m_element->Add();
				
		} else {
			// no vertices; element is discarded.
		}
		m_element = nullptr;
	}
}

//-----------------------------------------------------------------------------


}