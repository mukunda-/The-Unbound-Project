//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2015, Mukunda Johnson, All rights reserved. =========//

#include "stdafx.h"
#include "game/game.h"
#include "video/vertexbuffer.h"
#include "video/shaders/uishader.h"
#include "graphics/fontmaterial.h"
#include "graphics/vertexformats.h"
#include "video/shaders/linetester.h"
#include "system/variables.h"
#include "game/defs.h"
#include "graphics/material.h"

#include "game/gamecamera.h"

static const char BUILD_DATE[] = __DATE__ "/" __TIME__;

namespace Game {
	
namespace {
//-----------------------------------------------------------------------------
template <class m> void PrintMatrix3( m mat ) {
	printf( "\n%8.2f | %8.2f | %8.2f\n",
		mat(0,0),mat(1,0),mat(2,0) );

	printf( "%8.2f | %8.2f | %8.2f\n",
		mat(0,1),mat(1,1),mat(2,1) );

	printf( "%8.2f | %8.2f | %8.2f\n",
		mat(0,2),mat(1,2),mat(2,2) ); 
}

//-----------------------------------------------------------------------------
Video::VertexBuffer::ptr GenerateTestGeometry() {
	Graphics::VertexStream<Graphics::Vertex::Generic3D> verts; 
	
	// create a grid 
	for( int i = 0; i < 11; i++ ) {
		float scale =2.0;

		Graphics::Vertex::Generic3D v[4];
		v[0] = Graphics::Vertex::Generic3D( (-10.0f + (float)i*2) * scale, 0.0f, (-10.0f)*scale );
		v[1] = v[0];
		v[1].z = 10.0f * scale;

		v[2] = Graphics::Vertex::Generic3D( (-10.0f)*scale, 0.0f, (-10.0f + (float)i*2)*scale );
		v[3] = v[2];
		v[3].x = 10.0f * scale; 

		for( int i = 0; i < 4; i++ )
			verts.Push( v[i] );
	}

	auto buffer = Video::VertexBuffer::Create();
	verts.Load( *buffer );
	return buffer;
}
 
//-----------------------------------------------------------------------------
Video::VertexBuffer::ptr GenerateTestGeometry2() {
	using Graphics::Vertex::Texcola2D;
	Graphics::VertexStream<Texcola2D> verts; 
	verts.Push( Texcola2D(0.0,0.0,0.0,0.0,255,255,255,255) );
	verts.Push( Texcola2D(0.0,0.5,0.0,0.0,255,255,255,255) );
	verts.Push( Texcola2D(0.5,0.5,0.0,0.0,255,255,255,255) );
	verts.Push( Texcola2D(0.5,0.5,0.0,0.0,255,255,255,255) );
	verts.Push( Texcola2D(0.5,0.0,0.0,0.0,255,255,255,255) );
	verts.Push( Texcola2D(0.0,0.0,0.0,0.0,255,255,255,255) );
  
	auto buffer = Video::VertexBuffer::Create();
	verts.Load( *buffer );
	return buffer;
}

}

//-----------------------------------------------------------------------------
Game::Game() : 
	Module( "game", Levels::USER ),
	cl_master_address( System::Variables::Create( 
			"cl_master_address", "localhost", 
			"Address of master server." ))
{
	
}

//-----------------------------------------------------------------------------
Game::~Game() {

}

//-----------------------------------------------------------------------------
void Game::OnStart() {
	Video::RegisterShader<Shaders::LineTester>();
	Video::RegisterShader<Shaders::Ui>();

	auto mat = Graphics::CreateMaterial( "mymat", "linetest" );
	mat->SetParam( "color", "0.2 0.2 0.2" );
	
	Graphics::Element::ptr test_element = Graphics::CreateElement();
	test_element->Setup( GenerateTestGeometry(), Video::BlendMode::OPAQUE, 
		                 mat, 44, Video::RenderMode::LINES ); 
	test_element->Add();

	mat = Graphics::CreateMaterial( "mymat2", "ui" );
	
}

//-----------------------------------------------------------------------------
void Game::Run() {

	// connect to master
	/*
	m_net_master.Connect(
		cl_master_address.GetString(), 
		std::to_string( Game::PORT_CLIENT ) );
		*/
	
	//Video::Texture::Pointer tex = Video::Texture::New( "test" );	
	Graphics::FontMaterial testfont;
	testfont.LoadFace( "cour.ttf", 20 );
	testfont.LoadMaterial();


	GameCamera cam;
	cam.SetPosition( Eigen::Vector3f( 0, 0, 5.0 ) );
	cam.LookAt( Eigen::Vector3f( 0, 0, 0 ) );
	cam.UpdateVideo();
	
	float angle = 0.0;

	SDL_SetRelativeMouseMode( SDL_TRUE );
	
	SDL_Event e;
	bool quit = false;
	while (!quit){
		while (SDL_PollEvent(&e)){
			if (e.type == SDL_QUIT) {
				quit = true;
			}  

			if( e.type == SDL_KEYDOWN ) {
			
				// testing various camera orientations

				if( e.key.keysym.scancode == SDL_SCANCODE_Y ) {
					cam.SetOrientation( Eigen::Vector3f( 1,0,0 ) );
					 
				} else if( e.key.keysym.scancode == SDL_SCANCODE_U ) {
					cam.SetOrientation( Eigen::Vector3f( 0,1,0 ) );
				} else if( e.key.keysym.scancode == SDL_SCANCODE_I ) {
					cam.SetOrientation( Eigen::Vector3f( -1,0,0 ) );
				} else if( e.key.keysym.scancode == SDL_SCANCODE_H ) {
					cam.SetOrientation( Eigen::Vector3f( 0,0,1 ) );
				} else if( e.key.keysym.scancode == SDL_SCANCODE_J ) {
					cam.SetOrientation( Eigen::Vector3f( 0,-1,0 ) );
				} else if( e.key.keysym.scancode == SDL_SCANCODE_K ) {
					cam.SetOrientation( Eigen::Vector3f( 0,0,-1 ) );
				} else if( e.key.keysym.scancode == SDL_SCANCODE_O ) {
					cam.SetOrientation( Eigen::Vector3f( 1,1,0 ).normalized() );
				}
			}

			
		}

		
		int mx, my;
		SDL_GetRelativeMouseState( &mx, &my );

		if( SDL_GetKeyboardState(NULL)[SDL_SCANCODE_C] ) {
			mx = 20 ;
		} 
		 
		if( SDL_GetKeyboardState(NULL)[SDL_SCANCODE_ESCAPE] ) {
			quit = true;

		}

		cam.Pan( (float)mx, (float)my );  

		if( SDL_GetKeyboardState(NULL)[SDL_SCANCODE_W] ) {
			cam.MoveRel( Eigen::Vector3f( 0.0f, 0.0f, 0.1f ) );
		} else if( SDL_GetKeyboardState(NULL)[SDL_SCANCODE_S] ) {
			cam.MoveRel( Eigen::Vector3f( 0.0f, 0.0f, -0.1f ) );
		}

		if( SDL_GetKeyboardState(NULL)[SDL_SCANCODE_D] ) {
			cam.MoveRel( Eigen::Vector3f( 0.1f, 0.0f, 0.0f ) );
		} else if( SDL_GetKeyboardState(NULL)[SDL_SCANCODE_A] ) {
			cam.MoveRel( Eigen::Vector3f( -0.1f, 0.0f, 0.0f ) );
		}
		
		if( SDL_GetKeyboardState(NULL)[SDL_SCANCODE_Q] ) {
			cam.MoveRel( Eigen::Vector3f( 0.0f, 0.1f, 0.0f ) );
		} else if( SDL_GetKeyboardState(NULL)[SDL_SCANCODE_Z] ) {
			cam.MoveRel( Eigen::Vector3f( 0.0f, -0.1f, 0.0f ) );
		}

		if( SDL_GetKeyboardState(NULL)[SDL_SCANCODE_R] ) {
			cam.Rotate( Eigen::Vector3f(0.0f, 0.0f, -0.01f) );
		} else if( SDL_GetKeyboardState(NULL)[SDL_SCANCODE_T] ) {
			cam.Rotate( Eigen::Vector3f(0.0f, 0.0f, 0.01f) );
		}

		cam.OnTick();  
		cam.UpdateVideo();
		//Render the scene

		std::string build_string = "Unbound Development Build - ";
		build_string += BUILD_DATE;
		Gui::RenderText( testfont, 0, 20, 7, 16, build_string.c_str() );
		Gui::EndRendering();
		  
		Video::Clear();
		Graphics::RenderScene(); 
		Video::Swap();
		 
	}
}

}