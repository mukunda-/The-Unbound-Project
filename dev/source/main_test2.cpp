//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2015, Mukunda Johnson, All rights reserved. =========//

///////////////////////////////////////////////////////////////////////////////
// SANDBOX FOR DEVELOPING CLIENT STUFF
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
 
#include "system/system.h"
#include "system/debug_console.h"
#include "system/module.h"
#include "system/pulse.h"
#include "video/video.h"
#include "ui/ui.h"

//-----------------------------------------------------------------------------
class Test : public System::Module {

	System::Pulse m_pulse;

public:
	Test();

	void OnStart() override;
	void OnShutdown() override;

	void DoTick();
};

//-----------------------------------------------------------------------------
Test::Test() : 
		Module( "testapp", Levels::USER ), 
		m_pulse( 10.0 )  {
	
};

//-----------------------------------------------------------------------------
void Test::OnStart() {
	AddWork();

	Video::Open( 1000, 800 );
	//Ui::SetupScreen( 1000, 800 );

	m_pulse.Reset();
	DoTick();
}

//-----------------------------------------------------------------------------
void Test::DoTick() {

	static float r = 0.0;
	r = (float)fmod( r + 0.01, 1.0 );
	Video::SetBackgroundColor( r/2,r,1 ); 
	Video::Clear();


	Ui::Draw();

	Video::Swap();
	
	SDL_Event e;
	bool quit = false;

	static int mx, my;

	while( SDL_PollEvent(&e) ) {
		if( e.type == SDL_QUIT ) {
			quit = true;
		}
		 
		Ui::HandleInputEvent( e );
	}

	if( quit ) {
		System::Shutdown( "User exited." );
		
	} else {
		m_pulse.Wait( std::bind( &Test::DoTick, this ));
	}
}

//-----------------------------------------------------------------------------
void Test::OnShutdown() {
	RemoveWork();
}
 
//-----------------------------------------------------------------------------
INT WINAPI WinMain( HINSTANCE, HINSTANCE, LPSTR, INT ) {

	System::CreateDebugConsole();

	if( SDL_Init( SDL_INIT_VIDEO|SDL_INIT_EVENTS ) != 0 ) {
		std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
		return 0;
	}
	
	auto system = std::unique_ptr<System::Main>( new System::Main( 4 ));
	//System::RegisterModule<Net::Instance>();
	System::RegisterModule<Video::Instance>( "CLIENT" );
	System::RegisterModule<Ui::Instance>();
	System::RegisterModule<Test>();
	
	System::Start();
	
	SDL_Quit();
	return 0;
}
