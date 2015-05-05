//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2015, Mukunda Johnson, All rights reserved. =========//

#pragma once

//-----------------------------------------------------------------------------
namespace Ui { namespace Event {
	
//-----------------------------------------------------------------------------
enum class EventTypes {
	UNKNOWN       = 0x000,

	MOUSE_DOWN    = 0x100,
	MOUSE_UP,
	MOUSE_CLICK,
	MOUSE_MOTION,
	MOUSE_ENTER,
	MOUSE_LEAVE, 

	MOUSE_DRAG,
	MOUSE_DROP
};

/** ---------------------------------------------------------------------------
 * UI Event.
 */
class Event {

public:
	Event( int type );
	virtual ~Event();

	virtual EventTypes Type() { return EventTypes::UNKNOWN; }
};

#define UI_EVENT_TYPE( t ) virtual EventTypes Type() override { return t; }

//-----------------------------------------------------------------------------
enum class MouseButton {
	NONE,
	LEFT,
	MIDDLE,
	RIGHT,
	FOUR,
	FIVE
};

//-----------------------------------------------------------------------------
namespace {

	//-------------------------------------------------------------------------
	MouseButton ConvertSDLButton( int sdl_button ) {
		// convert sdl event mouse button index into our button index

		// (right now there is no change)
		return (MouseButton)sdl_button; 

	}

}

//-----------------------------------------------------------------------------
class MousePositionData {
	Eigen::Vector2i m_pos; 
	Eigen::Vector2i m_abs_pos;

public:
	MousePositionData( const Eigen::Vector2i &pos, 
		               const Eigen::Vector2i &abs_pos );
	
	/** -----------------------------------------------------------------------
	 * Returns cursor position relative to object.
	 */
	const Eigen::Vector2i &GetPosition() { return m_pos; }

	/** -----------------------------------------------------------------------
	 * Returns cursor position relative to screen.
	 */
	const Eigen::Vector2i &GetAbsPosition() { return m_abs_pos; }

};

//-----------------------------------------------------------------------------
class MouseButtonData {

	MouseButton m_button;

public:

	MouseButtonData( MouseButton button );
	
	/** -----------------------------------------------------------------------
	 * Get the button that was clicked.
	 */
	MouseButton GetButton() { return m_button; }

};

/** ---------------------------------------------------------------------------
 * Mouse cursor has moved.
 */
class MouseMotion : public MousePositionData, public Event {
	
public:
	
	MouseMotion( const Eigen::Vector2i &pos, 
		         const Eigen::Vector2i &abs_pos );

	UI_EVENT_TYPE( EventTypes::MOUSE_MOTION );
};

/** ---------------------------------------------------------------------------
 * Mouse has hovered over a region.
 */
class MouseEnter : public Event {
	
public:
	MouseEnter();

	UI_EVENT_TYPE( EventTypes::MOUSE_ENTER );
};

/** ---------------------------------------------------------------------------
 * Mouse has left a region.
 */
class MouseLeave : public Event {

public:
	MouseLeave();

	UI_EVENT_TYPE( EventTypes::MOUSE_LEAVE );
};

/** ---------------------------------------------------------------------------
 * Mouse pressed down and released on the region.
 */
class MouseClick : public MousePositionData, 
	               public MouseButtonData,
				   public Event {

public:
	MouseClick( const Eigen::Vector2i &pos, const Eigen::Vector2i &abs_pos,
		        MouseButton button );

	UI_EVENT_TYPE( EventTypes::MOUSE_CLICK );
};

/** ---------------------------------------------------------------------------
 * Mouse button pressed down.
 */
class MouseDown : public MousePositionData, 
	              public MouseButtonData,
				  public Event {

public:
	MouseDown( const Eigen::Vector2i &pos, const Eigen::Vector2i &abs_pos,
		       MouseButton button );

	UI_EVENT_TYPE( EventTypes::MOUSE_DOWN );
};

/** ---------------------------------------------------------------------------
 * Mouse button released.
 */
class MouseUp : public MousePositionData, 
	            public MouseButtonData,
				public Event {

public:
	MouseUp( const Eigen::Vector2i &pos, const Eigen::Vector2i &abs_pos,
		     MouseButton button );

	UI_EVENT_TYPE( EventTypes::MOUSE_UP ); 
};

class MouseDrag : public Event {
public:
	UI_EVENT_TYPE( EventTypes::MOUSE_DRAG ); 
};

class MouseDrop : public Event {
public:
	UI_EVENT_TYPE( EventTypes::MOUSE_DROP ); 
};

}}
