//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2015, Mukunda Johnson, All rights reserved. =========//

#include "stdafx.h"
 
#include "system/system.h"
#include "system/debug_console.h"
#include "system/module.h"
#include "system/pulse.h"
#include "video/video.h"

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
	SetBusy( true );
	Video::Open( 1000, 800 );

	m_pulse.Reset();
	DoTick();
}

//-----------------------------------------------------------------------------
void Test::DoTick() {

	static float r = 0.0;
	r = fmod( r + 0.01, 1.0 );
	Video::SetBackgroundColor( r/2,r,1 ); 
	Video::Clear();
	Video::Swap();
	
	SDL_Event e;
	bool quit = false;

	static int mx, my;

	while (SDL_PollEvent(&e)){
		if (e.type == SDL_QUIT) {
			quit = true;
		}

		int count = 0;
		if( e.type == SDL_MOUSEMOTION ) {
			mx = e.motion.x;
			my = e.motion.y;
			count++;
		//	Console::Print( "motion: %4d %4d", e.motion.x, e.motion.y );
		}

		Console::Print( "motion: %d", count );

		if( e.type == SDL_MOUSEBUTTONDOWN ) {
			Console::Print( "button: %4d %4d", e.button.x, e.button.y );
			if( e.button.x != mx || e.button.y != my ) {
				Console::Print( "Button/Motion mismatch" );
			}
		}
		if( e.type == SDL_MOUSEBUTTONUP ) {
			Console::Print( "button: %4d %4d", e.button.x, e.button.y );
			if( e.button.x != mx || e.button.y != my ) {
				Console::Print( "Button/Motion mismatch" );
			}
		}
	}

	if( quit ) {
		System::Shutdown( "User exited." );
		SetBusy( false );
	} else {
		m_pulse.Wait( std::bind( &Test::DoTick, this ));
	}
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
