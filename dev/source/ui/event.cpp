//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2015, Mukunda Johnson, All rights reserved. =========//

#include "stdafx.h"
#include "event.h"

namespace Ui {

namespace Event {

Event::Event( EventType type ) {
	m_type = type;
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



}}