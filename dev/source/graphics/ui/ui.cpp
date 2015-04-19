//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2015, Mukunda Johnson, All rights reserved. =========//

// gui.cpp
// core of the user interface
 
#include "stdafx.h"
#include "util/minmax.h"

#include "video/vertexbuffer.h"
#include "graphics/vertexformats.h"
#include "graphics/graphics.h"
#include "graphics/gui/gui.h"
#include "graphics/fontmaterial.h"

namespace Gui {
	
Instance *g_gui;

struct Gui {
	EIGEN_MAKE_ALIGNED_OPERATOR_NEW

	

	//-------------------------------------------------------------------------------------------------
	Gui() {
		m_focused_widget = nullptr;
		m_held_widget    = nullptr;
		m_hot_widget     = nullptr;
		m_held_button    = 0;
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
