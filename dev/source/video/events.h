//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2015, Mukunda Johnson, All rights reserved. =========//

#pragma once

#include "system/events/event.h"

//-----------------------------------------------------------------------------
namespace Video { namespace Events {

/** ---------------------------------------------------------------------------
 * Sent after the game window is created and initialized. 
 * It's safe to access the OpenGL context after this event.
 */
class VIDEO_OPENED : public System::Event {

	int m_width, m_height;

public:
	SYSTEM_DEFINE_EVENT( 0x010001, "VIDEO_OPENED", 0 );

	VIDEO_OPENED( int width, int height ) 
		: SYSTEM_EVENT_INIT, m_width(width), m_height(height) {};

	int ScreenWidth()  const { return m_width;  }
	int ScreenHeight() const { return m_height; }
};

/** ---------------------------------------------------------------------------
 * Sent after the game window is destroyed.
 */
class VIDEO_CLOSED : public System::Event {

public:
	SYSTEM_DEFINE_EVENT( 0x010002, "VIDEO_CLOSED", 0 );

	VIDEO_CLOSED() : SYSTEM_EVENT_INIT {};
};

}}
