//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright � 2015, Mukunda Johnson, All rights reserved. =========//


//-----------------------------------------------------------------------------
#include "stdafx.h"
#include "game/game.h"
#include "video/video.h"
#include "system/debug_console.h"
#include "graphics/graphics.h"

#include "net/core.h"

#include "system/system.h"

//-----------------------------------------------------------------------------
INT WINAPI WinMain( HINSTANCE hinst, HINSTANCE p1, LPSTR p2, INT p3 ) {

	System::CreateDebugConsole();

	if( SDL_Init( SDL_INIT_VIDEO|SDL_INIT_EVENTS ) != 0 ) {
		std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
		return 0;
	}
	
	auto system = std::unique_ptr<System::Main>( new System::Main(4) );
	System::RegisterModule<Net::Instance>();
	System::RegisterModule<Video::Instance>( "testes" );
	System::RegisterModule<Graphics::Instance>();
	System::RegisterModule<Game::Game>();
	System::Start();
	
	SDL_Quit();
	return 0;
}
