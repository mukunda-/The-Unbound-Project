//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright � 2016, Mukunda Johnson, All rights reserved. =========//

///////////////////////////////////////////////////////////////////////////////
// SANDBOX FOR DEVELOPING CLIENT STUFF
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
 
#include "system/system.h"
#include "system/debug_console.h"
#include "system/module.h"
#include "system/pulse.h"
#include "net/core.h"
#include "video/video.h"
#include "ui/ui.h"
#include "ui/test/coloredregion.h"
#include "video/shaders/uishader.h"

//-----------------------------------------------------------------------------
class Test : public System::Module {

	System::Pulse m_pulse;

public:
	Test();

	void OnStart() override;
	void OnShutdown() override;
	void OnFrame() override;

	void DoTick();
};

//-----------------------------------------------------------------------------
Test::Test() : 
		Module( "testapp", Levels::USER ), 
		m_pulse( 10.0 )  {
	
	Video::RegisterShader< Shaders::Ui >();
};

//-----------------------------------------------------------------------------
void Test::OnStart() {
	AddWork();

	Video::Open( 1000, 800 );
	Ui::SetupScreen( 1000, 800 );

	auto &r = Ui::Create<Ui::ColoredRegion>( "test_region1" );
	r.SetAllPoints();
 
}

//-----------------------------------------------------------------------------
void Test::OnFrame() {

	static float r = 0.0;
	r = (float)fmod( r + 0.01, 1.0 );
	Video::SetBackgroundColor( r/2,r,1 ); 
	Video::Clear();

	//::Console::Print( "time=%f", System::Time() );

	//Ui::Draw();

	auto buffer = Video::VertexBuffer::Create();
	auto mat = Graphics::CreateMaterial( "", "ui", nullptr );
	Graphics::Builder builder( Video::VertexBuffer::Usage::STREAM_DRAW, Video::RenderMode::TRIANGLES, Graphics::RenderLayer::UI );
	builder.New( mat );
	builder.AddVertex( 0, 0, 0, 0 );
	builder.AddVertex( 0, 0.1, 0, 0 );
	builder.AddVertex( 0.1, 0.1, 0, 0 );
	builder.AddVertex( 0.1, 0.1, 0, 0 );
	builder.AddVertex( 0.1, 0, 0, 0 );
	builder.AddVertex( 0, 0, 0, 0 );
	builder.Finish();
	
	Graphics::RenderScene();


	Video::Swap();
	
	SDL_Event e; 

	static int mx, my;

	while( SDL_PollEvent(&e) ) {
		if( e.type == SDL_QUIT ) {
			System::Shutdown( "User exited." ); 
		}
		 
		Ui::HandleInputEvent( e );
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
	System::RegisterModule<Net::Instance>();
	System::RegisterModule<Video::Instance>( "SANDBOX" );
	System::RegisterModule<Ui::Instance>();
	System::RegisterModule<Graphics::Instance>();
	System::RegisterModule<Test>();
	
	System::Start();
	
	SDL_Quit();
	return 0;
}
