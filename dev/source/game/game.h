//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2015, Mukunda Johnson, All rights reserved. =========//
#pragma once

// the Game class is created after the subsystem is initialized
// ie there is a game window and the actual game is starting

// todo need to not pass entire control to game class and 
// instead call it on ticks meanwhile
// keeping the subsystem updated

#include "graphics/gui/gui.h"
//#include "network/connection.h"
#include "system/variables.h"
#include "system/module.h"

namespace Game {

/// ---------------------------------------------------------------------------
/// Main game class.
///
class Game : public System::Module {
	  
	System::Variable &cl_master_address;

//	Network::Connection m_net_master;
	
public:
	Game();
	~Game();

	void Run();
};

}
