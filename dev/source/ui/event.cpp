//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2015, Mukunda Johnson, All rights reserved. =========//

#include "stdafx.h"
#include "event.h"
#include "region.h"

namespace Ui {

namespace Event {

//-----------------------------------------------------------------------------
Event::Event( EventType type ) {
	m_type = type;
}

//-----------------------------------------------------------------------------
MousePositionData::MousePositionData( const ivec2 &pos ) : 
		m_pos( pos ) {

}

//-----------------------------------------------------------------------------
ivec2 MousePositionData::GetPosition( const Region &r ) const {
	return m_pos - r.GetTopLeft();
}

//-----------------------------------------------------------------------------
MouseButtonData::MouseButtonData( MouseButton button ) : 
		m_button(button) {

}

//-----------------------------------------------------------------------------
MouseMotion::MouseMotion( const ivec2 &pos ) : 
		Event( EventType::MOUSE_MOTION ), MousePositionData( pos ) {
}

//-----------------------------------------------------------------------------
MouseEnter::MouseEnter() : Event( EventType::MOUSE_ENTER ) {}

//-----------------------------------------------------------------------------
MouseLeave::MouseLeave() : Event( EventType::MOUSE_LEAVE ) {}

//-----------------------------------------------------------------------------
Clicked::Clicked( const ivec2 &pos, MouseButton button ) :
		Event( EventType::CLICKED ), MousePositionData( pos ),
		MouseButtonData( button ) {
}

//-----------------------------------------------------------------------------
MouseDown::MouseDown( const ivec2 &pos, MouseButton button ) :
		Event( EventType::MOUSE_DOWN ), MousePositionData( pos ),
		MouseButtonData( button ) {
}

//-----------------------------------------------------------------------------
MouseUp::MouseUp( const ivec2 &pos, MouseButton button ) :
		Event( EventType::MOUSE_UP ), MousePositionData( pos ),
		MouseButtonData( button ) {
}

//-----------------------------------------------------------------------------
MouseDrag::MouseDrag() : Event( EventType::MOUSE_DRAG ) {}

//-----------------------------------------------------------------------------
MouseDrop::MouseDrop() : Event( EventType::MOUSE_DROP ) {}

//-----------------------------------------------------------------------------
Focused::Focused() : Event( EventType::FOCUSED ) {}

//-----------------------------------------------------------------------------
LostFocus::LostFocus() : Event( EventType::LOSTFOCUS ) {}


}}