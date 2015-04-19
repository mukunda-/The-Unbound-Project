//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2015, Mukunda Johnson, All rights reserved. =========//

#pragma once
 
#include "util/linkedlist.h"
#include "graphics/fontmaterial.h"
#include "graphics/builder.h"

//-----------------------------------------------------------------------------
namespace Ui {

//-----------------------------------------------------------------------------
enum class Alignment {
	CENTER,
	LEFT,
	RIGHT,

	TOP    = LEFT,
	BOTTOM = RIGHT
};

//-----------------------------------------------------------------------------
enum class Button {
	NONE,
	LEFT,
	MIDDLE,
	RIGHT,
	M4,
	M5
};

//-----------------------------------------------------------------------------
struct Event {
	enum {
		MOUSEDOWN,
		MOUSEUP,
		MOUSEMOVE,
		MOUSECLICK,

		KEYDOWN,
		KEYUP,
		TYPING,

		DRAW
	} type;


};

//-----------------------------------------------------------------------------
struct MouseEvent : public Event {
	Eigen::Vector2i pos;
	Eigen::Vector2i abs_pos;
	int button;
};


//-----------------------------------------------------------------------------
enum {
	EVENT_MOUSEDOWN,
	EVENT_MOUSEUP,

};

typedef boost::function< bool(const Event &) > EventHandler;

//-------------------------------------------------------------------------------------------------
class Widget : public Util::LinkedItem<Widget> {

	friend struct Gui;
protected:
	

	Eigen::Vector4i m_rect; // location relative to parent, and size
	Eigen::Vector4i m_abs_rect; // computed area in "screen" (the object) space 
	                            // after applying hierarchy
	bool m_mouseover;		// mouse is hovering over the object
	bool m_focusable;		// this object can take input focus by clicking on it or tabbing
	bool m_focused;			// this object has input focus
	
	bool m_pressed;			// the mouse is pressed and is in the object area
	



	//Font &font;			// font used to render text in this Widget

	Widget *m_parent;		// parent Widget or NULL for screen
	int m_align[2];			// alignment with parent Widget

	std::vector<Widget*> children;

	//int m_z_order;			// ordering of Widgets with their siblings TODO
							// negative values will appear behind the parent.

	EventHandler m_event_handler;

public:
	bool m_active;			// should appear on screen and accept events
	bool m_accept_events;	// should this Widget intercept events
	bool m_held;			// the mouse is holding this object
	bool m_hot;

	Widget();

	bool Picked( const Eigen::Vector2i &position );
	 
	// handle SDL event
	bool FireEvent( const Event &event );

	// find an Widget at this point
	//Widget *PickWidget( const Eigen::Vector2i &position );

	virtual void OnDraw();
};

bool HandleEvent( const SDL_Event &event );

void RenderText( Graphics::FontMaterial &font, int sort, int height, int x, int y, const char *text );
void EndRendering();

//-----------------------------------------------------------------------------
class Instance final {
	
	Widget m_screen; // screen widget, has no parent, parent of all.
	Widget *m_focused_widget;
	Widget *m_held_widget;
	Widget *m_hot_widget;
	int     m_held_button;
	Util::LinkedList<Widget> m_widgets;
	Eigen::Vector2i m_mouse_position;

	Graphics::Builder m_gfx_builder;

public:
	Instance();
	~Instance();



};

//-------------------------------------------------------------------------------------------------
}// namespace Gui

