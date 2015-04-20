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

//-----------------------------------------------------------------------------
namespace Ui {

using EventHandler = std::function< bool( const Event & ) >;


/** ---------------------------------------------------------------------------
 * Handle an input event from SDL.
 *
 * @param event SDL event to handle.
 */
bool HandleEvent( const SDL_Event &event );

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

/** ---------------------------------------------------------------------------
 * Called after everything is rendered.
 *
 * TODO should this be exposed?
 */
void EndRendering();

//-----------------------------------------------------------------------------
class Instance final : public System::Module {
	
	// screen region, has no parent, god region
	Region *m_screen; 

	// region that has input focus
	Region *m_focused_region;

	// region that is held by the mouse
	Region *m_held_widget;

	// region that is hovered over with the mouse button released
	Region *m_hot_widget;

	// button that is holding a region
	int     m_held_button;

	// list of ui objects
	Util::UniqueList<Object> m_objects;

	// mouse position
	Eigen::Vector2i m_mouse_position;

	// rendering buffer
	Graphics::Builder m_gfx_builder;
	
	std::vector<Object*> m_hot_objects;

public:
	EIGEN_MAKE_ALIGNED_OPERATOR_NEW

	Instance();
	~Instance();

	void RenderText( Graphics::FontMaterial &font, int sort, int height, 
		             int stroke, int x, int y, const Stref &text, 
					 float scale );

	void EndRendering();
};

static inline int ConvertSDLButton( int sdl_button ) {
	// convert sdl event mouse button index into our button index

	// (right now there is no change)
	return sdl_button; 

}



//-------------------------------------------------------------------------------------------------
} // namespace Ui

