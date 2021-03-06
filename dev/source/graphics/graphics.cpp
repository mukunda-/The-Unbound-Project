//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright � 2015, Mukunda Johnson, All rights reserved. =========//

// graphics subsystem

#include "stdafx.h" 
#include "graphics/graphics.h"
#include "graphics/material.h"
#include "console/console.h"
 
namespace Graphics {

Instance *g_instance = nullptr;

Element::Element() {
	//::Console::DebugPrint( "Element created.", (unsigned long)this );
}

Element::~Element() {
	//::Console::DebugPrint( "Element destroyed.", (unsigned long)this );

}

//-------------------------------------------------------------------------------------------------
void Element::Setup( const Video::VertexBuffer::ptr &buffer, Video::BlendMode blendmode, 
					 const MaterialPtr &mat, int buffer_size, Video::RenderMode render_mode ) {

	m_buffer = buffer;
	m_blend_mode = blendmode;
	m_material = mat;
	m_buffer_index = 0;
	m_buffer_offset = 0;
	m_buffer_start = 0;
	m_buffer_size = buffer_size;
	m_render_mode = render_mode;
}

//-------------------------------------------------------------------------------------------------
void Element::Add() {
	g_instance->AddElement( shared_from_this() );
}
/*
float sine[256];

void init_tables() {
	 
	for( int i =0 ; i < 256; i++ ) {
		sine[i] = (float)sin( ((double)i / 256.0) * 6.283185307179586476925286766559 );
	}
	 
}


float tablecos( int rot ) {
	return sine[(64-rot)&255];
}

float tablesin( int rot ) {
	return sine[rot&255];
}*/

//particle *solid_particles;
//element *unsorted_elements;
//particle *unsorted_particles;

//#define sorted_list_count 512

//element *sorted_elements[sorted_list_count]; // for bucket sorting, i = 512 / (1+z/16)
//particle *sorted_particles[sorted_list_count];

//#define particle_verts_max 40000 // multiple of 4.
//#define particles_max 10000

//Video::generic_vertex particle_vertices_solid[particle_verts_max];
//Video::generic_vertex particle_vertices_blended[particle_verts_max];
//Video::generic_vertex particle_vertices_blended_sorted[particle_verts_max]; // switch to an index buffer later

//Video::VertexBuffer blended_particles_buffer;
//Video::VertexBuffer solid_particles_buffer;

//particle particles[particles_max];

//int solid_particle_verts_total;
//int blended_particle_verts_total;
//int particles_total;

/*
void compute_viewing_planes() {
	viewplane_h = Video::GetFarPlane()[3] - Video::GetFarPlane()[0];
	viewplane_v = Video::GetFarPlane()[0] - Video::GetFarPlane()[1];

	viewplane_h.normalize();
	viewplane_v.normalize();
}

void reset_lists() { 
	for( int i = 0; i < sorted_list_count; i++ ) {
		sorted_elements[i] = 0;
		sorted_particles[i] = 0;
	}
	unsorted_elements = 0;
	unsorted_particles = 0;
	solid_particles = 0;
	//sorted_particles_compiled = 0;
}
 
 
void add_unsorted_particle( particle *a ) {
	a->link = unsorted_particles;
	unsorted_particles = a;
}
*/
/*
void add_particle( particle *a ) {
	if( a->blend_mode != Video::BLEND_OPAQUE ) {
		a->link = unsorted_particles;
		unsorted_particles = a;
	} else {
		a->link = solid_particles;
		solid_particles = a;
		//if( solid_particles_b ) {
		//	solid_particles_b->link = a;
		//	solid_particles_b = a;
		//	
		//} else {
		//	solid_particles_a = solid_particles_b = a;
		//	
		//}
		//a->link = 0;
	}
}*/

/*

rendering procedure:

alpha sort blended elements

split elements into shaders and material groups


*/

/*
//-------------------------------------------------------------------------------------------------
void render_element( const element *e ) {
	if( Video::Shaders::GetActive() != e->shader ) {
		e->shader->Use();
		//Video::SetShader( e->shader );

		if( e->shader == Video::Shaders::Find("objects") ) {//Video::SHADER_OBJECTS ) {
			Video::Shaders::ObjectShader *s = e->shader;
			s->SetCameraMatrix( Video::GetXPMatrix()->data() );
			s->SetTranslation( e->translate[0], e->translate[1], e->translate[2] );
			s->SetSampler( 0 );
			s->SetSkylight( 1.0, 1.0, 1.0, 0.0 );

			//Video::SetObjectShaderCamera();
			//Video::SetObjectShaderTranslation(e->translate[0],e->translate[1],e->translate[2]);
			//Video::SetObjectShaderSampler(0);
			//Video::SetObjectShaderSunlight( 255,255,255,0);
		}
	}
	
	if( Video::GetBlendMode() != e->blend_mode ) {
		Video::SetBlendMode(e->blend_mode);
		
	}

	Textures::Bind( e->texture );

	e->buffer->RenderData( e->render_mode, e->buffer_offset, e->buffer_start, e->buffer_size, e->buffer_index );
}

//-------------------------------------------------------------------------------------------------
void draw_element( element *a ) {
	if( a->blend_mode != Video::BLEND_OPAQUE ) {
		a->link = unsorted_elements;
		unsorted_elements = a;
	} else {
		render_element(a);
	}
}*/

//-------------------------------------------------------------------------------------------------
/*
void sort_particles() {
	
	for( particle *a = unsorted_particles; a; ) {
		particle *n = a->link;
		float findex = 8192.0f / (16.0f + a->depth); // 1 / (1+z/16)
		int index = (int)floor(findex+0.5);
		//if( index < 0 ) index = 0;
		if( index > sorted_list_count-1 ) index = sorted_list_count-1;
		a->link = sorted_particles[index];
		sorted_particles[index] = a;
		a = n;
	}
}

//-------------------------------------------------------------------------------------------------
void sort_elements() {
	
	for( element *a = unsorted_elements; a; ) {
		element *n = a->link;
		float findex = 8192.0f / (16.0f + a->depth); // 1 / (1+z/16)
		int index = (int)floor(findex+0.5);
		
		a->link = sorted_elements[index];
		sorted_elements[index] = a;

		a = n;
	}
}

//-------------------------------------------------------------------------------------------------
void sort_blended_particle_vertices() {
	// PARTS BLEND1, PARTS BLEND2, PARTS BLEND3, ELEMENTS

	//dummy_particle.link = 0;
	int write = 0;
	for( int i = 0; i < sorted_list_count; i++ ) {
		for( particle *p = sorted_particles[i]; p; p = p->link ) {
			int index = p->data_index;
			memcpy( (void*)(particle_vertices_blended_sorted + write), (void*)(particle_vertices_blended + index), sizeof( Video::generic_vertex ) * 4 );
			write += 4;
		}
	}
	blended_particles_buffer.BufferData( particle_vertices_blended_sorted, write * sizeof( Video::generic_vertex ), GL_STREAM_DRAW_ARB );
}

//-------------------------------------------------------------------------------------------------
void render_solid_particles() {

	element e;
	e.blend_mode = Video::BLEND_OPAQUE;
	e.buffer = &solid_particles_buffer;
	e.buffer_index = 0;
	e.buffer_offset = 0;
	e.buffer_size = solid_particle_verts_total;
	e.buffer_start = 0;
	e.render_mode = GL_QUADS;
	e.shader = Video::Shaders::Find( "objects" );//Video::SHADER_OBJECTS;
	e.texture = Textures::PARTICLES;
	e.translate[0] = e.translate[1] = e.translate[2] = 0.0f;

	render_element( &e );


}

void render_blended_particle_set( int start, int count, u8 blending ) {
	element e;
	e.blend_mode = blending;
	e.buffer = &blended_particles_buffer;
	e.buffer_index = 0;
	e.buffer_offset = 0;
	e.buffer_size = count;
	e.buffer_start = start;
	e.render_mode = GL_QUADS;
	e.shader = Video::Shaders::Find( "objects" ); //Video::SHADER_OBJECTS;
	e.texture = Textures::PARTICLES;
	e.translate[0] = e.translate[1] = e.translate[2] = 0.0f;

	Video::SetDepthBufferMode( Video::ZBUFFER_READONLY );

	render_element( &e );

	Video::SetDepthBufferMode( Video::ZBUFFER_ENABLED );
}
*/
/*
//-------------------------------------------------------------------------------------------------
void render_blended_elements() {
	int particle_start = 0, particle_count = 0, previous_blend_mode = 0;
	for( int i = 0; i < sorted_list_count; i++ ) {
		for( particle *p = sorted_particles[i]; p; p = p->link ) {
			if( p->blend_mode != previous_blend_mode ) {
				if( particle_count != 0 ) {
					render_blended_particle_set( particle_start, particle_count, previous_blend_mode );
					particle_start += particle_count;
					particle_count = 0;
				}
			}
			previous_blend_mode = p->blend_mode;
			particle_count += 4;
		}
		for( element *e = sorted_elements[i]; e; e = e->link ) {
			if( particle_count != 0 ) {
				render_blended_particle_set( particle_start, particle_count, previous_blend_mode );
				particle_start += particle_count;
				particle_count = 0;
			}

			render_element( e );
			
		}
		
	}

	if( particle_count != 0 ) {
		render_blended_particle_set( particle_start, particle_count, previous_blend_mode );
		particle_start += particle_count;
		particle_count = 0;
	}
}

void new_scene() {
	reset_lists();
	solid_particle_verts_total = 0;
	blended_particle_verts_total = 0;
	particles_total = 0;
}*/


/*
void sorted_rendering() {

	render_solid_particles();

	sort_particles();
	sort_elements();

	sort_blended_particle_vertices();

	render_blended_elements();
}*/

/*
void finalize_data() {
	sort_particles();
	sort_elements();
	sort_blended_particle_vertices();

	
	solid_particles_buffer.BufferData( particle_vertices_solid, solid_particle_verts_total * sizeof( Video::generic_vertex ), GL_STREAM_DRAW_ARB );
}

void render_solid_graphics() {
	render_solid_particles();
}

void render_blended_graphics() {
	render_blended_elements();
}
*/

//-------------------------------------------------------------------------------------------------
/*
void add_vertex( float x, float y, float z, float u, float v, u8 r, u8 g, u8 b, u8 a, bool blend ) {
	Video::generic_vertex *w;
	if( blend ) {
		w = particle_vertices_blended+blended_particle_verts_total;
		blended_particle_verts_total++;
	} else {
		w = particle_vertices_solid+solid_particle_verts_total;
		solid_particle_verts_total++;
	}
	w->x = x;
	w->y = y;
	w->z = z;
	w->u = u;
	w->v = v;
	w->r = r;
	w->g = g;
	w->b = b;
	w->a = a;
}


void draw_sprite( cml::vector3f position, float width, float height, float u1, float v1, float u2, float v2, u8 blending, u8 r, u8 g, u8 b, u8 a, int rot, bool vertical ) {
	
	cml::vector3f vertices[4];
	u1 += 0.001f;
	u2 -= 0.001f;
	v1 += 0.001f;
	v2 -= 0.001f;

	float c, s;

	c = cos( (float)rot * 3.14159265f * 2.0f / 65536.0f );
	s = sin( (float)rot * 3.14159265f * 2.0f / 65536.0f );

	width *= 0.5;
	height *= 0.5;

	float x1 = c * -width - s * height;
	float y1 = s * -width + c * height;

	float x2 = c * -width - s * -height;
	float y2 = s * -width + c * -height;

	float x3 = c * width - s * -height;
	float y3 = s * width + c * -height;

	float x4 = c * width - s * height;
	float y4 = s * width + c * height;

	if( vertical ) {
		vertices[0] = position + (x1 * viewplane_h);// + (y1 * cml::vector3f(0,1,0));
		vertices[1] = position + (x2 * viewplane_h);// + (y2 * viewplane_v);
		vertices[2] = position + (x3 * viewplane_h);// + (y3 * viewplane_v);
		vertices[3] = position + (x4 * viewplane_h);// + (y4 * viewplane_v);
		vertices[0][1] += y1;
		vertices[1][1] += y2;
		vertices[2][1] += y3;
		vertices[3][1] += y4;

	} else {
		vertices[0] = position + (x1 * viewplane_h) + (y1 * viewplane_v);
		vertices[1] = position + (x2 * viewplane_h) + (y2 * viewplane_v);
		vertices[2] = position + (x3 * viewplane_h) + (y3 * viewplane_v);
		vertices[3] = position + (x4 * viewplane_h) + (y4 * viewplane_v);

	}
	
	if( blending == Video::BLEND_OPAQUE ) {
		if( solid_particle_verts_total == (particle_verts_max) ) return;
		add_vertex( vertices[0][0], vertices[0][1], vertices[0][2], u1, v1, r,g,b,a, false );
		add_vertex( vertices[1][0], vertices[1][1], vertices[1][2], u1, v2, r,g,b,a, false );
		add_vertex( vertices[2][0], vertices[2][1], vertices[2][2], u2, v2, r,g,b,a, false );
		add_vertex( vertices[3][0], vertices[3][1], vertices[3][2], u2, v1, r,g,b,a, false );
		
	} else {

		if( particles_total == particle_verts_max ) return;
		if( blended_particle_verts_total == (particle_verts_max) ) return;
		
		particles[particles_total].blend_mode = blending;
		particles[particles_total].data_index = blended_particle_verts_total;

		float depth = Video::GetCameraDistance( position );
		particles[particles_total].depth = depth;
		add_unsorted_particle( &particles[particles_total] );
		particles_total++;

		add_vertex( vertices[0][0], vertices[0][1], vertices[0][2], u1, v1, r,g,b,a, true );
		add_vertex( vertices[1][0], vertices[1][1], vertices[1][2], u1, v2, r,g,b,a, true );
		add_vertex( vertices[2][0], vertices[2][1], vertices[2][2], u2, v2, r,g,b,a, true );
		add_vertex( vertices[3][0], vertices[3][1], vertices[3][2], u2, v1, r,g,b,a, true );

	}

}*/



//-----------------------------------------------------------------------------
Instance::Instance() 
		: Module( "graphics", Module::Levels::GL ) {

	assert( g_instance == nullptr );
	g_instance = this;

	int error = FT_Init_FreeType( &m_ftlib );
	if( error ) {
		Console::PrintErr( "Error starting FreeType: %i", error );
	}
}

//-----------------------------------------------------------------------------
Instance::~Instance() {
	g_instance = nullptr;

	FT_Done_FreeType( m_ftlib );
}

//-----------------------------------------------------------------------------
void Instance::RenderList( Util::SharedList<Element> &list ) {
	
	Element::ptr next;
	Element::ptr e;

	for( e = list.First(); e; e = next ) {
		
		next = e->NextLink<Element>();

		e->GetMaterial()->Bind();

		int offset, start, size, index;

		e->GetBufferParams( &index, &offset, &start, &size );

		e->GetBuffer()->Render( e->GetRenderMode(), 
			                    offset, start, size, index );

		if( e->GetPostAction() == Element::PostAction::REMOVE ) {
			list.Pull( e );
		}
	}
}

//-----------------------------------------------------------------------------
void Instance::RenderScene() {
	Video::SetBlendMode( Video::BlendMode::OPAQUE );
	RenderList( m_elements_opaque );

	Video::SetBlendMode( Video::BlendMode::ALPHA );
	Video::SetDepthBufferMode( Video::DepthBufferMode::DISABLED );

	// todo sort ui elements
	RenderList( m_elements_ui );

}

//-----------------------------------------------------------------------------
void Instance::AddElement( const Element::ptr &e ) {
	if( e->GetRenderLayer() == RenderLayer::OBJECTS ) {
		if( e->GetBlendMode() == Video::BlendMode::OPAQUE ) {
			m_elements_opaque += e;
		} else {
			m_elements_blended += e;
		}
	} else if( e->GetRenderLayer() == RenderLayer::UI ) {
		m_elements_ui += e;
	}
}

//-----------------------------------------------------------------------------
Material::ptr Instance::CreateMaterial( const Stref &name, 
	                                    const Stref &shader,
										MaterialInitializer setup ) {
	if( name.Empty() ) {
		// Unnamed material.

		auto ptr = std::make_shared<Material>( shader );
		if( setup ) setup( ptr, shader );
		return ptr;
	}
	
	// Named material.

	if( !name.Empty() ) {
		if( m_materials.count( name ) > 0 ) {
			// Already created; return existing.
			return m_materials.at( name );
		}
	}

	// Create new material.
	auto ptr = std::make_shared<Material>( shader );
	m_materials[name] = ptr;

	// Initialize if a function is given.
	if( setup ) {
		setup( ptr, shader );
	}

	return ptr;
}

//-----------------------------------------------------------------------------
void Instance::DeleteMaterial( const Stref &name ) {
	
	if( m_materials.count( name ) == 0 ) {
		Console::PrintErr( "DeleteMaterial: Name not found." );
		return;
	}

	m_materials.erase( name );
}

//-----------------------------------------------------------------------------
Material::ptr Instance::GetMaterial( const Stref &name ) {
	try {
		return m_materials.at( name );
	} catch( std::out_of_range & ) {
		return nullptr;
	}
}

//-----------------------------------------------------------------------------
Element::ptr Instance::CreateElement() {
	return Element::Create();
}

//-----------------------------------------------------------------------------
FT_Library    FTLib()                                          { return g_instance->FTLib();                }
Material::ptr CreateMaterial( CStref &n, CStref &s, MaterialInitializer f ) { return g_instance->CreateMaterial( n, s, f ); }
void          DeleteMaterial( CStref &n )                      { g_instance->DeleteMaterial( n );           }
void          RenderList( Util::SharedList<Element> &l )       { g_instance->RenderList( l );               }  
void          RenderScene()                                    { g_instance->RenderScene();                 }
Element::ptr  CreateElement()                                  { return g_instance->CreateElement();        }

} // namespace Graphics
