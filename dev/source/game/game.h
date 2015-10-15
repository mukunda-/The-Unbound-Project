//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2015, Mukunda Johnson, All rights reserved. =========//
#pragma once 

// the Game class is created after the subsystem is initialized
// ie there is a game window and the actual game is starting

// todo need to not pass entire control to game class and 
// instead call it on ticks meanwhile
// keeping the subsystem updated
  
#include "system/variables.h"
#include "system/module.h"

#include "gamecamera.h"

namespace Game {

/// ---------------------------------------------------------------------------
/// Main game class.
///
class Game : public System::Module {
	  
	System::Variable &cl_master_address;

	GameCamera m_cam;

	//Graphics::font_material 
//
//	Network::Connection m_net_master;
	
public:
	Game();
	~Game();

	//void OnLoad() override;
	void OnStart() override;
	void OnFrame() override;
	//void OnShutdown() override;

	void Run();
};

}
 