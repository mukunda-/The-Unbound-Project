//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2015, Mukunda Johnson, All rights reserved. =========//

#pragma once

#include "util/linkedlist.h"
#include "util/uniquelist.h"
#include "graphics/fontmaterial.h"
#include "graphics/builder.h"
#include "system/module.h"
#include "forwards.h"
#include "event.h"

//-----------------------------------------------------------------------------
namespace Ui {

//using EventHandler = std::function< bool( const Event & ) >;

/** ---------------------------------------------------------------------------
 * Handle an input event from SDL.
 *
 * @param event SDL event to handle.
 * @returns true if the event interacted with the ui, false if it should
 *          be passed to the game.
 */
bool HandleInputEvent( const SDL_Event &event );

/** ---------------------------------------------------------------------------
 * Render text onto the screen.
 *
 * @param font   Font to use.
 * @param sort   Z order.
 * @param height Height of font to use.
 * @param x      X pixel offset.
 * @param y      Y pixel offset.
 * @param text   Text to render.
 */
void RenderText( Graphics::FontMaterial &font, int sort, int height, 
	             int x, int y, const Stref &text );

//-----------------------------------------------------------------------------
class Instance final : public System::Module {

	friend class Object;
	friend class Region;
	
	// screen region, has no parent, god region
	std::unique_ptr<Region> m_screen;

	// the currently picked region, updated in ApplyMousePosition
	// this will never be null, and will default to the screen
	Region *m_picked_region = nullptr;

	// region that has input focus
	Region *m_focused_region = nullptr;

	// region that is pressed by the mouse
	Region *m_pressed_region = nullptr; 

	// button that is pressed
	MouseButton m_pressed_button = MouseButton::NONE;

	// master list of objects 
	std::vector<Object*> m_objects;

	// list of regions
	std::vector<Region*> m_regions;

	// mouse position
	ivec2 m_mouse_position;
	ivec2 m_mouse_position_new;

	// rendering buffer
	Graphics::Builder m_gfx_builder; 

	std::vector<SDL_Event> m_input_events;

	// the region that started a compute operation
	// this is to guard against circular anchoring
	Region *m_computing_region = nullptr;

	void UpdateMousePosition( int x, int y );
	void ApplyMousePosition();

	void ReleaseFocus();
	void SetFocus( Region &region );

	void ResetPressed();
	void SetPressed( Region &region, MouseButton button );

	void OnObjectCreated( Object &object );
	void OnObjectDeleted( Object &object );

	void OnRegionCreated( Region &region );
	void OnRegionDeleted( Region &region );

	Region &GetScreen() { return *m_screen; }

public:
	EIGEN_MAKE_ALIGNED_OPERATOR_NEW

	Instance();
	~Instance();

	void RenderText( const Graphics::FontMaterial &font, int sort, int height, 
		             int stroke, int x, int y, const Stref &text, 
					 float scale );

	void EndRendering();

	Region *PickRegion( const ivec2 &pos );

	bool HandleInputEvent( const SDL_Event &sdlevent );
	void FinishInputEvents();
};


//-----------------------------------------------------------------------------
} // namespace Ui

