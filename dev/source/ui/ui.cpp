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
#include "event.h"
#include "console/console.h"
#include "video/events.h"

//-----------------------------------------------------------------------------
namespace Ui {
	
Instance *g_ui;

//-----------------------------------------------------------------------------
MouseButton ConvertSDLButton( int sdl_button ) {
	// convert sdl event mouse button index into our button index

	// (right now there is no change)
	return (MouseButton)sdl_button; 

}

//-----------------------------------------------------------------------------
Instance::Instance() : System::Module( "ui", Levels::USER ), 
					   m_gfx_builder( Video::VertexBuffer::Usage::STREAM_DRAW, 
					                  Video::RenderMode::TRIANGLES, 
									  Graphics::RenderLayer::UI ) {
	g_ui = this;

	m_screen = new Region( "Screen" );
	m_screen->SetupScreen( 0, 0 );

	m_picked_region = m_screen;
}

//-----------------------------------------------------------------------------
Instance::~Instance() {

	m_cleaning_up = true;

	for( auto i : m_objects ) {
		delete i.second;
	} 

	g_ui = nullptr;
}

//-----------------------------------------------------------------------------
void Instance::OnPrepare() {
	
}

//-----------------------------------------------------------------------------
void Instance::SetupScreen( int width, int height ) {
	m_screen->SetupScreen( width, height );
}
	
//-----------------------------------------------------------------------------
void Instance::EndRendering() {
	m_gfx_builder.Finish();
}

//-----------------------------------------------------------------------------
Region *Instance::PickRegion( const ivec2 &pos ) {

	int sortmax = -0x10000;
	Region *highest = m_screen;

	for( auto r : m_regions ) {
		if( r->Inside( pos ) ) {
			
			if( r->GetComputedStrata() > sortmax ) {
				sortmax = r->GetComputedStrata();
				highest = r;
			}
		}
	}

	return highest;
}

//-----------------------------------------------------------------------------
void Instance::UpdateMousePosition( int x, int y ) {

	Eigen::Vector2i pos( x, y );
	m_mouse_position_new = pos - m_screen->GetTopLeft();
}

//-----------------------------------------------------------------------------
void Instance::ApplyMousePosition() {
	if( m_mouse_position != m_mouse_position_new ) {
		m_mouse_position = m_mouse_position_new;
	}

	for( auto r : m_regions ) {
		r->m_mouseover_last = r->m_mouseover;
		r->m_mouseover = false;
	}

	{
		Region *r = PickRegion( m_mouse_position );
		m_picked_region = r;

		for( ; r != nullptr; r = r->m_parent ) {
			if( r->m_clickable ) {
				r->m_mouseover = true;
			}
		}
	}

	Event::MouseMotion motion_event( m_mouse_position );

	for( auto r : m_regions ) {

		if( r->m_mouseover != r->m_mouseover_last ) {
			
			if( r->m_mouseover ) {
				r->SendEvent( Event::MouseEnter() );
			} else {
				r->SendEvent( Event::MouseLeave() );
			}
		}

		if( r->m_mouseover && r->m_clickable ) {
			
			r->SendEvent( motion_event );
		}
	}

}

//-----------------------------------------------------------------------------
void Instance::ResetPressed() {
	if( m_pressed_region ) {
		auto r = m_pressed_region;
		m_pressed_region = nullptr;

		r->m_pressed = false;
		r->SendEvent( Event::MouseUp( m_mouse_position, m_pressed_button ));
		 
	}
}

//-----------------------------------------------------------------------------
void Instance::SetPressed( Region &region, MouseButton button ) {
	if( m_pressed_region ) {
		ResetPressed();
	}

	m_pressed_region = &region;
	m_pressed_button = button;
	region.m_pressed = true;

	region.SendEvent( Event::MouseDown( m_mouse_position, button ));
}

//-----------------------------------------------------------------------------
void Instance::ReleaseFocus() {
	if( m_focused_region ) {
		auto r = m_focused_region;
		m_focused_region = nullptr;

		r->m_focused = false;
		r->SendEvent( Event::LostFocus() );
	} 
}

//-----------------------------------------------------------------------------
void Instance::SetFocus( Region &r ) {
	if( m_focused_region == &r ) return;

	if( m_focused_region ) {
		ReleaseFocus();
	}

	m_focused_region = &r;
	r.SendEvent( Event::Focused() );

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
	  motion event, but handle that as if it can happen,
	  ignoring motion events if there is a click event, making the above
	  sequence something like

	  (motion)-(motion)-(click)-(motionevent)-(clickevent)...
	  -(more motion)-(motionevent)

	  from testing it appears the motion events are condensed when you
	  poll/pump the sdl events
	*/

	if( sdlevent.type == SDL_MOUSEBUTTONDOWN ) {
		UpdateMousePosition( sdlevent.button.x, sdlevent.button.y ); 
		ApplyMousePosition();
		 
		if( m_picked_region->IsFocusable() ) {
			SetFocus( *m_picked_region );
		}
		
		SetPressed( *m_picked_region, ConvertSDLButton( sdlevent.button.button ));
	
		return true;

	} else if( sdlevent.type == SDL_MOUSEBUTTONUP ) {

		UpdateMousePosition( sdlevent.button.x, sdlevent.button.y ); 
		ApplyMousePosition();

		if( m_pressed_region ) {

			if( m_pressed_region->Inside( m_mouse_position )) {
				m_pressed_region->SendEvent( 
						Event::Clicked( m_mouse_position, m_pressed_button ));
			}

			ResetPressed();
		}

		return true;

	} else if( sdlevent.type == SDL_MOUSEMOTION ) {

		UpdateMousePosition( sdlevent.button.x, sdlevent.button.y ); 

		return true;
	} 
	
	// filter out unhandled events
	return false;
}
 
//-----------------------------------------------------------------------------
void Instance::FinishInputEvents() {
	ApplyMousePosition();

}

//-----------------------------------------------------------------------------
Object &Instance::CreateObject( const Stref &name, const ObjectFactory &factory ) {
	if( m_objects.count( name ) > 0 ) {
		throw Error::NameCollision();
	}

	if( name.Empty() ) {
		throw Error::BadName();
	}

	// todo: additional name checks?

	auto obj = factory();

	return *obj;
}

//-----------------------------------------------------------------------------
void Instance::OnObjectCreated( Object &obj ) {
	
	assert( m_objects.count(obj.GetName()) == 0 );
	m_objects[obj.GetName()] = &obj;
}

//-----------------------------------------------------------------------------
void Instance::OnObjectDeleted( Object &obj ) {

	if( m_cleaning_up ) return;

	/*
	for( auto i = m_objects.begin(); i != m_objects.end(); i++ ) {
		if( *i == &obj ) {
			m_objects.erase(i);
			return;
		}
	}*/

	if( obj.GetName() != "" ) {
		m_objects.erase( obj.GetName() );
	}

	assert( !"OnObjectDeleted object not found." );
}

//-----------------------------------------------------------------------------
void Instance::OnRegionCreated( Region &r ) {
	m_regions.push_back( &r );
}

//-----------------------------------------------------------------------------
void Instance::OnRegionDeleted( Region &r ) {

	if( m_cleaning_up ) return;

	if( m_picked_region == &r ) {
		m_picked_region = m_screen;
	}

	if( m_pressed_region == &r ) {
		m_pressed_region = nullptr;
	}

	if( m_focused_region == &r ) {
		m_focused_region = nullptr;
	}
	
	for( auto i = m_regions.begin(); i != m_regions.end(); i++ ) {
		if( *i == &r ) {
			m_regions.erase( i );
			return;
		}
	}
	
	assert( !"OnRegionDeleted region not found." );
}

//-----------------------------------------------------------------------------
void Instance::Draw() {

	for( auto &r : m_regions ) {
		r->Draw();
	}

	EndRendering();
}

//-----------------------------------------------------------------------------
Instance &Instance::Get() {
	return *g_ui;
}

//-----------------------------------------------------------------------------
void EndRendering()                    { g_ui->EndRendering();                         }
void SetupScreen( int w, int h )       { g_ui->SetupScreen( w, h );                    }
Instance &GetInstance()                { return *g_ui;                                 }
void Draw()                            { g_ui->Draw();                                 }
bool HandleInputEvent( const SDL_Event &e ) { return g_ui->HandleInputEvent( e );      }

//-----------------------------------------------------------------------------
}
