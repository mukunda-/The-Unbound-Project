//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2015, Mukunda Johnson, All rights reserved. =========//

#include "stdafx.h"
 
#include "system/system.h"
#include "system/debug_console.h"
#include "system/module.h"
#include "video/video.h"

//-----------------------------------------------------------------------------
class Test : public System::Module {
public:
	Test();

	void OnStart() override;
	void OnShutdown() override;
};

//-----------------------------------------------------------------------------
Test::Test() : Module( "testapp", Levels::USER ) {

};

//-----------------------------------------------------------------------------
void Test::OnStart() {
	Video::Open( 1000, 800 );
}

//-----------------------------------------------------------------------------
void Test::OnShutdown() {

}
 
//-----------------------------------------------------------------------------
INT WINAPI WinMain( HINSTANCE hinst, HINSTANCE p1, LPSTR p2, INT p3 ) {

	System::CreateDebugConsole();

	if( SDL_Init( SDL_INIT_VIDEO|SDL_INIT_EVENTS ) != 0 ) {
		std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
		return 0;
	}
	
	auto system = std::unique_ptr<System::Main>( new System::Main( 4 ));
	//System::RegisterModule<Net::Instance>();
	System::RegisterModule<Video::Instance>();
	System::RegisterModule<Test>();
	System::Start();
	
	SDL_Quit();
	return 0;
}
