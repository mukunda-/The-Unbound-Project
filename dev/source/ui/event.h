//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2015, Mukunda Johnson, All rights reserved. =========//

#pragma once

#include "forwards.h"

//-----------------------------------------------------------------------------
namespace Ui { 
	
enum class MouseButton {
	NONE,
	LEFT,
	MIDDLE,
	RIGHT,
	FOUR,
	FIVE
};

namespace Event {
	
//-----------------------------------------------------------------------------
enum class EventType {
	UNKNOWN       = 0x000,

	MOUSE_DOWN    = 0x100,
	MOUSE_UP,
	MOUSE_MOTION,
	MOUSE_ENTER,
	MOUSE_LEAVE, 
	CLICKED,

	MOUSE_DRAG,
	MOUSE_DROP,

	FOCUSED       = 0x200,
	LOSTFOCUS
};

/** ---------------------------------------------------------------------------
 * UI Event.
 */
class Event {

	EventType m_type;

public:
	Event( EventType type );
	virtual ~Event();

	EventType Type() const { return m_type; }
}; 

//-----------------------------------------------------------------------------
class MousePositionData {

	ivec2 m_pos;

public:
	MousePositionData( const ivec2 &pos );
	
	/** -----------------------------------------------------------------------
	 * Returns cursor position.
	 */
	const ivec2 &GetPosition() const { return m_pos; }
	
	/** -----------------------------------------------------------------------
	 * Returns cursor position relative to a region.
	 */
	ivec2 GetPosition( const Region &r ) const;
};

//-----------------------------------------------------------------------------
class MouseButtonData {

	MouseButton m_button;

public:

	MouseButtonData( MouseButton button );
	
	/** -----------------------------------------------------------------------
	 * Get the button that was clicked.
	 */
	MouseButton GetButton() const { return m_button; }

};

/** ---------------------------------------------------------------------------
 * Mouse cursor has moved.
 */
class MouseMotion : public MousePositionData, public Event {
	
public:
	
	MouseMotion( const ivec2 &pos );
};

/** ---------------------------------------------------------------------------
 * Mouse has hovered over a region.
 */
class MouseEnter : public Event {
	
public:
	MouseEnter();
};

/** ---------------------------------------------------------------------------
 * Mouse has left a region.
 */
class MouseLeave : public Event {

public:
	MouseLeave();
};

/** ---------------------------------------------------------------------------
 * Mouse pressed down and released on the region.
 */
class Clicked : public MousePositionData, 
	               public MouseButtonData,
				   public Event {

public:
	Clicked( const ivec2 &pos, MouseButton button ); 
};

/** ---------------------------------------------------------------------------
 * Mouse button pressed down.
 */
class MouseDown : public MousePositionData, 
	              public MouseButtonData,
				  public Event {

public:
	MouseDown( const ivec2 &pos, MouseButton button );
};

/** ---------------------------------------------------------------------------
 * Mouse button released.
 */
class MouseUp : public MousePositionData, 
	            public MouseButtonData,
				public Event {

public:
	MouseUp( const Eigen::Vector2i &pos, MouseButton button );
};

/** ---------------------------------------------------------------------------
 * Picked up.
 */
class MouseDrag : public Event {
public:
	MouseDrag();
};

/** ---------------------------------------------------------------------------
 * Dropped.
 */
class MouseDrop : public Event {
public:
	MouseDrop();
};

/** ---------------------------------------------------------------------------
 * Gained input focus.
 */
class Focused : public Event {
public:
	Focused();
};

/** ---------------------------------------------------------------------------
 * Lost input focus.
 */
class LostFocus : public Event {
public:
	LostFocus();
};

}}
