//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

#include "stdafx.h"
#include "game/game.h"
#include "video/vertexbuffer.h"
#include "video/shaders/uishader.h"
#include "graphics/fontmaterial.h"
#include "graphics/vertexformats.h"
#include "video/shaders/linetest.h"
#include "system/variables.h"
#include "system/defs.h"

#include "game/gamecamera.h"

static const char BUILD_DATE[] = __DATE__ "/" __TIME__;

namespace Game {

Game::Game() : 
	cl_master_address(
		System::Variable::Create( 
			"cl_master_address", System::Variable::T_STRING,
			"localhost", "Address of master server." ) )
{
	
}

Game::~Game() {
}

template <class m> void PrintMatrix3( m mat ) {
	printf( "\n%8.2f | %8.2f | %8.2f\n",
		mat(0,0),mat(1,0),mat(2,0) );

	printf( "%8.2f | %8.2f | %8.2f\n",
		mat(0,1),mat(1,1),mat(2,1) );

	printf( "%8.2f | %8.2f | %8.2f\n",
		mat(0,2),mat(1,2),mat(2,2) ); 
}

Video::VertexBuffer::Pointer GenerateTestGeometry() {
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

	Video::VertexBuffer::Pointer buffer = Video::VertexBuffer::Create();
	verts.Load( *buffer );
	return buffer;
}
 

Video::VertexBuffer::Pointer GenerateTestGeometry2() {
	Graphics::VertexStream<Graphics::Vertex::Texcola2D> verts; 
	verts.Push( Graphics::Vertex::Texcola2D(0.0,0.0,0.0,0.0,255,255,255,255) );
	verts.Push( Graphics::Vertex::Texcola2D(0.0,0.5,0.0,0.0,255,255,255,255) );
	verts.Push( Graphics::Vertex::Texcola2D(0.5,0.5,0.0,0.0,255,255,255,255) );
	verts.Push( Graphics::Vertex::Texcola2D(0.5,0.5,0.0,0.0,255,255,255,255) );
	verts.Push( Graphics::Vertex::Texcola2D(0.5,0.0,0.0,0.0,255,255,255,255) );
	verts.Push( Graphics::Vertex::Texcola2D(0.0,0.0,0.0,0.0,255,255,255,255) );
  
	Video::VertexBuffer::Pointer buffer = Video::VertexBuffer::Create();
	verts.Load( *buffer );
	return buffer;
}
 
void Game::Run() {

	// connect to master
	 
	m_net_master.Connect(
		cl_master_address.GetString(), 
		std::to_string( System::PORT_CLIENT ) );


	Shaders::LineTester shader;
	Shaders::Ui ui_shader;
	
	Graphics::Material mymat( "linetest" );
	Graphics::Material mymat2( "ui" );
	mymat.SetParam( "color", "0.2 0.2 0.2" );

	//Video::Texture::Pointer tex = Video::Texture::New( "test" );	
	Graphics::FontMaterial testfont;
	testfont.LoadFace( "cour.ttf", 20 );
	testfont.LoadMaterial();


	Graphics::Element test_element;
	Graphics::SetupElement( test_element, GenerateTestGeometry(), 
		Video::BLEND_OPAQUE, mymat, 44, GL_LINES );
	

	Graphics::AddElement( test_element );

	GameCamera cam;
	cam.SetPosition( Eigen::Vector3f( 0, 0, 5.0 ) );
	cam.LookAt( Eigen::Vector3f( 0,0,0 ) );
	cam.UpdateVideo();
	  
	float angle = 0.0;


	SDL_SetRelativeMouseMode(SDL_TRUE);
	
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