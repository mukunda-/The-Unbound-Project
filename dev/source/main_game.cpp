//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//


//------------------------------------------------------------------------------------------------------
#include "stdafx.h"
#include "include_game.h"

//------------------------------------------------------------------------------------------------------
void RunGame() {
	boost::scoped_ptr<Game> game(new Game);
	game->Run();
}

//------------------------------------------------------------------------------------------------------
INT WINAPI WinMain( HINSTANCE hinst, HINSTANCE p1, LPSTR p2, INT p3 ) {

//int main( char *argc, char *argv[] ) {
	//glewInit();
	//glfwInit();
	
	if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_EVENTS) != 0){
		std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
		return 0;
	}
	
	Video::Open( 1200, 675 );
	//Video::SetBackgroundColor(0,0,0);

	RunGame();
 
	Video::Close();
	SDL_Quit();
	return 0;
}
