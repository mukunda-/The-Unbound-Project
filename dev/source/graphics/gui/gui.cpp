//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//

// gui.cpp
// core of the user interface

//hi

#include "stdafx.h"
#include "util/minmax.h"

#include "video/vertexbuffer.h"
#include "graphics/vertexformats.h"
#include "graphics/graphics.h"
#include "graphics/gui/gui.h"
#include "graphics/fontmaterial.h"

namespace Gui {
	
struct Gui {
	EIGEN_MAKE_ALIGNED_OPERATOR_NEW

	class GfxStream {

	private:

		Graphics::Material *m_mat;
		int m_sort;

		int m_buffer_start;
		int m_buffer_size;
		
		Graphics::Element *m_element;

		Graphics::VertexStream<Graphics::Vertex::Texcola2D> m_vertex_data;
		Video::VertexBuffer::Pointer m_vbo;

		/**
		 * Output the current graphics element being built and start a new one.
		 *
		 * \param mat Material to use for the new element.
		 * \param sort sorting order for the new element.
		 */
		void Split( Graphics::Material &mat, int sort ) {
			OutputElement();
			
			m_mat = &mat;
			m_sort = sort;
			m_buffer_size = 0;

			// setup default Split settings
			m_element = new Graphics::Element;
			m_element->m_autoremove = Graphics::Element::AR_DELETE;
			m_element->m_blend_mode = Video::BLEND_ALPHA;
			m_element->m_buffer = m_vbo;
			m_element->m_buffer_index = 0;
			m_element->m_buffer_offset = 0;
			m_element->m_buffer_size = 0;
			m_element->m_buffer_start = m_buffer_start;
			m_element->m_sort = sort;
			m_element->m_material = m_mat;
			m_element->m_render_mode = GL_TRIANGLES;
			m_element->m_layer = Graphics::LAYER_UI;
		}
		
		/**
		 * Output the current element being built and reset the pointer.
		 *
		 */
		void OutputElement() {
			if( m_element ) {
				if( m_buffer_size != 0 ) {
					m_element->m_buffer_size = m_buffer_size;
					m_buffer_start += m_buffer_size;
					Graphics::AddElement( *m_element );
				} else {
					delete m_element;
				}
				m_element = nullptr;
			}
		}

	public:
		/**
		 * Construct a new GfxStream object.
		 *
		 * GfxStreams are used to buffer vertices and output
		 * them to elements.
		 */
		GfxStream()  {
			//
			m_element = nullptr;
			m_vbo = Video::VertexBuffer::Create( GL_STREAM_DRAW );
			m_buffer_start = 0;
			m_buffer_size = 0;
		}

		/**
		 * Start a new sub-element in this stream.
		 *
		 * \param mat Material to use for rendering the upcoming vertice.s
		 * \param sort sorting order when rendering this object
		 */
		void Start( Graphics::Material &mat, int sort ) {

			// start is called before adding more vertices
			// it splits the element if the material or sorting dont match
			//
			if( !m_element || m_element->m_material != &mat || m_element->m_sort != sort ) {
				Split( mat, sort );
			}
			
		}

		/**
		 * End is called after EVERYTHING is loaded, to output the final element being worked on
		 * and load the vertex buffer.
		 * 
		 * End is not called for each Start called, only at the very end.
		 */
		void End() {
			// end is called after everything is added, to add the final element
			// and load the vertex buffer
			OutputElement();

			m_vertex_data.Load( *m_vbo );

			m_buffer_start = 0;
			m_buffer_size = 0;
		}

		/**
		 * Add a vertex to the current element.
		 */
		void AddVertex( float x, float y, float u, float v, uint8_t r, uint8_t g, uint8_t b, uint8_t a ) {
			m_vertex_data.Push( Graphics::Vertex::Texcola2D( x, y, u, v, r, g, b, a ) );
			m_buffer_size++;
		}
		
		/**
		 * Add a vertex to the current element.
		 */
		void AddVertex( float x, float y, float u, float v  ) {
			m_vertex_data.Push( Graphics::Vertex::Texcola2D( x, y, u, v, 255,255,255,255 ) );
			m_buffer_size++;
		}
	};
	
	Widget m_screen; // screen widget, has no parent, parent of all.
	Widget *m_focused_widget;
	Widget *m_held_widget;
	Widget *m_hot_widget;
	int  m_held_button;
	Util::LinkedList<Widget> m_widgets;
	Eigen::Vector2i m_mouse_position;

	GfxStream m_gfx_stream;

	//-------------------------------------------------------------------------------------------------
	Gui() {
		m_focused_widget = nullptr;
		m_held_widget = nullptr;
		m_hot_widget = nullptr;
		m_held_button = 0;
		 
	}

	//-------------------------------------------------------------------------------------------------
	static int ConvertSDLButton( int sdl_button ) {
		// convert sdl event mouse button index into our button index

		// (right now there is no change)
		return sdl_button;
	}

	//-------------------------------------------------------------------------------------------------
	void RenderText( Graphics::FontMaterial &font, int sort, int height, int stroke, 
					int x, int y, const char *text, float scale ) {

		const Graphics::Font::CharacterSet *font_charset = font.GetCharacterSet( height, stroke );
		if( !font_charset ) return;

		m_gfx_stream.Start( font.m_material, sort );

		y += font_charset->GetHeight() / 4;

		
		float penx, peny, scalex, scaley;
		scalex = 1.0f / (float)Video::ScreenWidth();
		scaley = 1.0f / (float)Video::ScreenHeight();
		penx = (float)x * scalex;
		peny = (float)y * scaley;

	

		while( *text ) {
			char c = *text;

			float u,v,u2,v2;
			u = (float)font_charset->GetCharacter(c)->x / 512.0f;
			v = (float)font_charset->GetCharacter(c)->y / 512.0f;
			u2 = u + (float)font_charset->GetCharacter(c)->w / 512.0f;
			v2 = v + (float)font_charset->GetCharacter(c)->h / 512.0f;

			float x1, y1, x2, y2;
			x1 = penx + (float)font_charset->GetCharacter(c)->left * scalex * scale;
			y1 = peny - (float)font_charset->GetCharacter(c)->top * scaley * scale;
			x2 = x1 + (float)font_charset->GetCharacter(c)->w * scalex * scale;
			y2 = y1 + (float)font_charset->GetCharacter(c)->h * scaley * scale;
		
		
			m_gfx_stream.AddVertex( x1, y1, u,  v );
			m_gfx_stream.AddVertex( x1, y2, u,  v2 );
			m_gfx_stream.AddVertex( x2, y2, u2, v2 );
			m_gfx_stream.AddVertex( x2, y2, u2, v2 );
			m_gfx_stream.AddVertex( x2, y1, u2, v );
			m_gfx_stream.AddVertex( x1, y1, u,  v );
		
			penx += ((float)font_charset->GetCharacter(c)->advance / 64.0f) * scalex * scale;

			text++;
		}
	}

	void EndRendering() {
		m_gfx_stream.End();
	}


	//-------------------------------------------------------------------------------------------------
	void ResetHot() {
		if( m_hot_widget ) {
			m_hot_widget->m_hot = false;
			m_hot_widget = 0;
		}
	}
	
	//-------------------------------------------------------------------------------------------------
	void SetHot( Widget &e ) {
		ResetHot();
		m_hot_widget = &e;
		e.m_hot = true;
	}

	//-------------------------------------------------------------------------------------------------
	void ResetHold() {
		if( m_held_widget ) {
			m_held_widget->m_held = false;
		}
		m_held_widget = 0;
	}

	//-------------------------------------------------------------------------------------------------
	void HoldWidget( Widget &e, int button ) {
		ResetHold();
		m_held_widget = &e;
		e.m_held = true;
		m_held_button = button;
	}
	
	//-------------------------------------------------------------------------------------------------
	void ReleaseFocus() {
		if( m_focused_widget ) {
			m_focused_widget->m_focused = false;
		}
		m_focused_widget = nullptr;
	}

	//-------------------------------------------------------------------------------------------------
	void SetFocus( Widget &e ) {
		ReleaseFocus();
		m_focused_widget = &e;
	}
	
	//-------------------------------------------------------------------------------------------------
	Widget *PickWidget( const Eigen::Vector2i &position ) {
		// find a widget that is touching this position
		//
		for( auto e = m_widgets.GetFirst(); e; e = e->m_next ) {
			if( e->m_active && e->m_accept_events && e->Picked( position ) ) {
				return e;
			}
		}
		return nullptr;
	}

	//-------------------------------------------------------------------------------------------------
	// pass SDL events here.
	bool HandleEvent( const SDL_Event &sdlevent ) {
	
		if( sdlevent.type == SDL_MOUSEBUTTONDOWN ) {
			m_mouse_position[0] = sdlevent.button.x - m_screen.m_rect[0];
			m_mouse_position[1] = sdlevent.button.y - m_screen.m_rect[1];

			ReleaseFocus();
			ResetHold();
			Widget *e = PickWidget( m_mouse_position );
			if( e ) {
				if( e->m_focusable ) {
					SetFocus(*e);
				}
				
				HoldWidget(*e, ConvertSDLButton( sdlevent.button.button ) );
				e->m_pressed = true;
				e->m_held = true;

				MouseEvent event;
				event.abs_pos = m_mouse_position;
				event.pos[0] = m_mouse_position[0] - e->m_abs_rect[0];
				event.pos[1] = m_mouse_position[1] - e->m_abs_rect[1];
				event.button = ConvertSDLButton( sdlevent.button.button );
				event.type = Event::MOUSEDOWN;
				
				e->FireEvent( event );
				return true;
			}
			return false;
		} else if( sdlevent.type == SDL_MOUSEBUTTONUP ) {
			m_mouse_position[0] = sdlevent.button.x;
			m_mouse_position[1] = sdlevent.button.y;
			 
			if( m_held_widget ) {
				
				MouseEvent event;
				event.abs_pos = m_mouse_position;
				event.pos[0] = m_mouse_position[0] - m_held_widget->m_abs_rect[0];
				event.pos[1] = m_mouse_position[1] - m_held_widget->m_abs_rect[1];
				event.button = ConvertSDLButton( sdlevent.button.button );
	 
				event.type = Event::MOUSEUP;

				m_held_widget->FireEvent( event );

				if( m_held_widget->Picked( m_mouse_position ) ) {
					
					event.type = Event::MOUSECLICK;
					m_held_widget->FireEvent( event );
					return true;
				}
			}
			return false;
		} else if( sdlevent.type == SDL_MOUSEMOTION ) {
			m_mouse_position[0] = sdlevent.motion.x;
			m_mouse_position[1] = sdlevent.motion.y;

			Widget *e = PickWidget( m_mouse_position );
			if( e ) {
				SetHot(*e);
			} else {
				ResetHot();
			}

			if( m_held_widget ) {

				MouseEvent event;
				event.abs_pos = m_mouse_position;
				event.button = m_held_button;
				event.pos[0] = m_mouse_position[0] - m_held_widget->m_abs_rect[0];
				event.pos[1] = m_mouse_position[1] - m_held_widget->m_abs_rect[1];
				event.type = Event::MOUSEMOVE;
				m_held_widget->FireEvent( event );
				return true;
			} else {
				Widget *e = PickWidget( m_mouse_position );
				if( e ) {

					MouseEvent event;
					event.abs_pos = m_mouse_position;
					event.button = BUTTON_NONE;
					event.pos[0] = m_mouse_position[0] - e->m_abs_rect[0];
					event.pos[1] = m_mouse_position[1] - e->m_abs_rect[1];		
					e->FireEvent( event );	
					return false;
				}
				return false;
			} 
			
		} 
	
		// filter out unhandled events
 
		return false;
	}
};

//-------------------------------------------------------------------------------------------------
Gui *g_gui = nullptr; 

Interface::Interface() {
	g_gui = new Gui;
}

Interface::~Interface() {
	delete g_gui;
}
 
//-------------------------------------------------------------------------------------------------
void RenderText( Graphics::FontMaterial &font, int sort, int height, int x, int y, const char *text ) {
	g_gui->RenderText( font, sort, height, 0, x, y, text, 1.0 );
}

//-------------------------------------------------------------------------------------------------
void EndRendering() {
	g_gui->EndRendering();
}

//-------------------------------------------------------------------------------------------------
bool HandleEvent( const SDL_Event &event ) {
	return g_gui->HandleEvent( event );
}

//-------------------------------------------------------------------------------------------------
}
