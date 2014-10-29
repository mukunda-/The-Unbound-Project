//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//


//------------------------------------------------------------------------------------------------------
#include "stdafx.h"
#include "game/game.h"
#include "video/video.h"

//------------------------------------------------------------------------------------------------------
void RunGame() {
	std::unique_ptr<Game::Game> game(new Game::Game);
	game->Run();
}

//------------------------------------------------------------------------------------------------------
INT WINAPI WinMain( HINSTANCE hinst, HINSTANCE p1, LPSTR p2, INT p3 ) {

//int main( char *argc, char *argv[] ) {
	//glewInit();
	//glfwInit();

	System::Init i_system(2);
	
	{
		if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_EVENTS) != 0){
			std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
			return 0;
		}

		Network::Init i_network(2);
	
		Video::Open( 1200, 675 );

		RunGame();

		Video::Close();
		SDL_Quit();
	}
	return 0;
}
