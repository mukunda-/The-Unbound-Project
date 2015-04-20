//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2015, Mukunda Johnson, All rights reserved. =========//

#pragma once

//-----------------------------------------------------------------------------
namespace Ui {

//-----------------------------------------------------------------------------
struct Event {
	enum class Type {
		MOUSEDOWN,
		MOUSEUP,
		MOUSEMOVE,
		MOUSECLICK,

		KEYDOWN,
		KEYUP,
		TYPING,

		DRAW
	};

	Type type;
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
struct MouseEvent : public Event {
	Eigen::Vector2i pos;
	Eigen::Vector2i abs_pos;
	Button button;
};

}