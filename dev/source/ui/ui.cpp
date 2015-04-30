//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2015, Mukunda Johnson, All rights reserved. =========//

// core of the user interface
 
#include "stdafx.h"
#include "util/minmax.h"

#include "graphics/vertexformats.h"
#include "graphics/graphics.h"
#include "graphics/builder.h"
#include "graphics/fontmaterial.h"
#include "ui.h"
#include "region.h"

namespace Ui {
	
Instance *g_ui;

//-----------------------------------------------------------------------------
Instance::Instance() : System::Module( "ui", Levels::USER ), 
					   m_gfx_builder( Video::VertexBuffer::Usage::STREAM_DRAW, 
					                  Video::RenderMode::TRIANGLES, 
									  Graphics::RenderLayer::UI ) {
	
	m_focused_region = nullptr;
	m_held_region    = nullptr;
	m_held_button    = 0;
}

//-----------------------------------------------------------------------------
Instance::~Instance() {

}
	
//-----------------------------------------------------------------------------
void Instance::RenderText( Graphics::FontMaterial &font, int sort, int height, 
	                       int stroke, int x, int y, const Stref &text, 
						   float scale ) {

	const auto *font_charset = font.GetCharacterSet( height, stroke );
	if( !font_charset ) return;

	m_gfx_builder.New( font.m_material, sort );

	y += font_charset->GetHeight() / 4;

	float penx, peny, scalex, scaley;
	scalex = 1.0f / (float)Video::ScreenWidth();
	scaley = 1.0f / (float)Video::ScreenHeight();
	penx = (float)x * scalex;
	peny = (float)y * scaley;

	const char *ctext = *text;

	while( *ctext ) {
		char c = *ctext;

		const auto *cter = font_charset->GetCharacter(c);

		float u,v,u2,v2;
		u =      (float)cter->x / 512.0f;
		v =      (float)cter->y / 512.0f;
		u2 = u + (float)cter->w / 512.0f;
		v2 = v + (float)cter->h / 512.0f;

		float x1, y1, x2, y2;
		x1 = penx + (float)cter->left * scalex * scale;
		y1 = peny - (float)cter->top  * scaley * scale;
		x2 = x1   + (float)cter->w    * scalex * scale;
		y2 = y1   + (float)cter->h    * scaley * scale;
		
		m_gfx_builder.AddVertex( x1, y1, u,  v  );
		m_gfx_builder.AddVertex( x1, y2, u,  v2 );
		m_gfx_builder.AddVertex( x2, y2, u2, v2 );
		m_gfx_builder.AddVertex( x2, y2, u2, v2 );
		m_gfx_builder.AddVertex( x2, y1, u2, v  );
		m_gfx_builder.AddVertex( x1, y1, u,  v  );
		
		penx += ((float)cter->advance / 64.0f) * scalex * scale;

		ctext++;
	}
}

//-----------------------------------------------------------------------------
void Instance::EndRendering() {
	m_gfx_builder.Finish();
}

//-----------------------------------------------------------------------------
void Instance::UpdateMousePosition( int x, int y ) {

	Eigen::Vector2i pos( x, y );
	m_mouse_position_new = pos - m_screen->GetAbsRect().pos;
}
 

//-----------------------------------------------------------------------------
void Instance::FinishInputEvents() {
	// remove redundant mouse events

}
 
//-----------------------------------------------------------------------------
bool Instance::HandleInputEvent( const SDL_Event &sdlevent ) {
	/*
	  some notes about the input handling

	  mouse input should be buffered until a game tick happens
	  at that point all of the input events are processed at once

	  multiple mouse clicks may occur on a single tick (although unlikely).

	  clicks should never be lost!

	  multiple mouse motion events should be condensed into a single
	  mouse motion event, the mouse position may change between events
	  on the same tick if click events are mixed in, 
	  e.g. (motion)-(motion)-(event)-(click)-(event)-(more motion)-(event)

	  not sure if the click events can change the position from the last
	  motion event, but handle that as if it can happen, maybe also
	  ignoring motion events if there is a click event, making the above
	  sequence something like

	  (motion)-(motion)-(click)-(motionevent)-(clickevent)...
	  -(more motion)-(motionevent)

	*/

	if( sdlevent.type == SDL_MOUSEBUTTONDOWN ) {
		UpdateMousePosition( sdlevent.button.x, sdlevent.button.y ); 

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
#endif

#if 0
	
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
	
#endif
//-----------------------------------------------------------------------------
void RenderText( Graphics::FontMaterial &f, int s, int h, int x, int y, const Stref &t ) 
                                       { g_ui->RenderText( f, s, h, 0, x, y, t, 1.0 ); }
void EndRendering()                    { g_ui->EndRendering();                         }
//bool HandleEvent( const SDL_Event &e ) { return g_ui->HandleInputEvent( e );           }


//-----------------------------------------------------------------------------
}
