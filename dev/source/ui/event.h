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
class MousePositionEvent : public Event {
	Eigen::Vector2i m_pos; 
	Eigen::Vector2i m_abs_pos;

public:
	MousePositionEvent( const Eigen::Vector2i &pos, 
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

/** ---------------------------------------------------------------------------
 * Mouse cursor has moved.
 */
class MouseMotion : public MousePositionEvent {
	
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
class MouseClick : public MousePositionEvent {

	MouseButton m_button;

public:
	MouseClick( const Eigen::Vector2i &pos, const Eigen::Vector2i &abs_pos,
		        MouseButton button );

	MouseButton GetButton() { return m_button; }

	UI_EVENT_TYPE( EventTypes::MOUSE_CLICK );
};

/** ---------------------------------------------------------------------------
 * Mouse button pressed down.
 */
class MouseDown : public MousePositionEvent {

public:
	MouseDown( const Eigen::Vector2i &pos, const Eigen::Vector2i &abs_pos,
		       MouseButton button );

	UI_EVENT_TYPE( EventTypes::MOUSE_DOWN );
};

/** ---------------------------------------------------------------------------
 * Mouse button released.
 */
class MouseUp : public MousePositionEvent {

public:
	MouseUp( const Eigen::Vector2i &pos, const Eigen::Vector2i &abs_pos,
		     MouseButton button );

	UI_EVENT_TYPE( EventTypes::MOUSE_UP ); 
};

}}
