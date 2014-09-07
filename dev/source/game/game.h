//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//
#pragma once

// the Game class is created after the subsystem is initialized
// ie there is a game window and the actual game is starting

// todo need to not pass entire control to game class and instead call it on ticks meanwhile
// keeping the subsystem updated

#include "graphics/gui/gui.h"

class Game : public Gui::Interface {
	
public:
	Game();
	~Game();

	void Run();
};

