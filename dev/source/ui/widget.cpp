//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//
#include "stdafx.h"
#include "gui.h"

namespace Gui {

//-------------------------------------------------------------------------------------------------
Widget::Widget() {
	m_parent = 0;
	m_mouseover = false;
	m_focused = false;
	m_held = false;
	m_pressed = false;
	m_align[0] = 0;
	m_align[1] = 0;
	m_accept_events = false;
	m_active = false;
}

 

//-------------------------------------------------------------------------------------------------
bool Widget::FireEvent( const Event &event ) {
	if( !m_accept_events ) return false;

	if( m_event_handler ) {
		return m_event_handler( event );
	} else {
	
		return false;
	}
}
 
bool Widget::Picked( const Eigen::Vector2i &position ) {
	return PointInsideRect( position, m_abs_rect );
}

void Widget::OnDraw() {

}

}