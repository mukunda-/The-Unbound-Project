//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//

#include "stdafx.h"

#include "debugsettings.h"

TCHAR g_strFont[LF_FACESIZE];

// debug
//namespace Scene { extern int image_intensity; }

HINSTANCE g_hinst;
const char *g_cmdline;
int g_argc;
char **g_argv;

//-------------------------------------------------------------------------------------------------
int oldtime=0;
int fps_counter=0;
int fps=0;

//-------------------------------------------------------------------------------------------------
void fps_record() {
	fps_counter++;
	int newtime = timeGetTime();
	if( newtime - oldtime > 1000 ) {
		oldtime = newtime;
		fps=fps_counter;
		fps_counter=0;
	}
}

//-------------------------------------------------------------------------------------------------
int angle = 0;

float FOV = 45.0;

float poopything = 0.0;


void initializemap();
extern int debug_instance_count;
extern int debug_count_2;

enum {
	SERVERNODE_NULL,
	SERVERNODE_MASTER,
	SERVERNODE_WORLD,
	SERVERNODE_PROCESS
};


//Game::Engine *game_engine;

float poopy = 0.0;

Video::Texture *test_texture;
Video::VertexBuffer *test_vb;

//-------------------------------------------------------------------------------------------------
void WindowLoopFrame() {
	poopy += 0.01;
	//poopy = 1.14 - poopy;
	if( poopy >= 6.283185307179586476925286766559 ) poopy -= 6.283185307179586476925286766559;
	//if( poopy >= 2.0 ) poopy -= 2.0;
	float r,g,b;
//	g = poopy < 1.0 ? poopy : 2.0-poopy;
	g = abs(sin(poopy));
	r = g * 0.3;
	b = g * 0.9;
	g = g * 0.5;
	Video::SetBackgroundColor( r,g,b );
	Video::Clear();
	Video::SetCamera_LookAt( cml::vector3f( 2, 2, 2 ), cml::vector3f( 0,0,0 ) );
	       
	Video::Shaders::ObjectShader *objshader = static_cast<Video::Shaders::ObjectShader*>(Video::Shaders::Find( "objshader" ));
	assert( objshader );
	objshader->Use();
	objshader->SetCameraMatrix( Video::GetXPMatrix()->data());
	objshader->SetSampler(0);
	objshader->SetFog( 500.0, 1.0 );
	objshader->SetSkylight( 1.0, 1.0, 1.0, 0.0 );
	objshader->SetTranslation( 0.0, 0.0, 0.0 );
//	objshader->
	objshader->SetVertexAttributePointers();

	Video::SetActiveTextureSlot(0);
	Video::FindTexture( "moontest" )->Bind();

	test_vb->Bind();
	Video::DrawQuads( 0, 4 );
	///wopgpojjopgpjoopwagopwgopjewgapojgaweojpawegjopawegjopawegoawegjop
	//Video::Swap();
	Sleep(1);
}

//-------------------------------------------------------------------------------------------------
INT ClientEntryPoint() {
	return 0;
}

//-------------------------------------------------------------------------------------------------
int ServerEntryPoint() {
	char arg[64];
	char cmdline[1024];
	Util::CopyString( cmdline, g_cmdline );

	int nodetype = SERVERNODE_NULL;

	while( true ) {
		Util::ScanArgString( cmdline, arg, sizeof arg ); 
		if( Util::StrEmpty(arg) ) break;

		if( Util::StrEqual( arg, "-master" ) ) {
			// this is the master node
			if( nodetype != SERVERNODE_NULL ) {
				printf( "Error: Multiple nodetypes specified! Can not, process... Too much..." );
				return EXIT_FAILURE;
			}
			nodetype = SERVERNODE_MASTER;
			} else if( Util::StrEqual( arg, "-world" ) ) {
				if( nodetype != SERVERNODE_NULL ) {
			printf( "Error: Multiple nodetypes specified! Have mercy!" );
				return EXIT_FAILURE;
			}
			nodetype = SERVERNODE_WORLD;
		} else if( Util::StrEqual( arg, "-process" ) ) {
			if( nodetype != SERVERNODE_NULL ) {
				printf( "Error: Multiple nodetypes specified. -_-" );
				return EXIT_FAILURE;
			}
			nodetype = SERVERNODE_PROCESS;
		}
	}

	if( nodetype == SERVERNODE_NULL ) {
		printf( "Error: Nodetype not specified!" );
		return EXIT_FAILURE;
	}

	if( nodetype == SERVERNODE_MASTER ) {
		System::ServerMasterNode *program = new System::ServerMasterNode();
		program->Run();
		delete program;
	}

	return 0;
}

//-------------------------------------------------------------------------------------------------
INT WinMainREAL( HINSTANCE h_inst, HINSTANCE, LPSTR cmdline, INT ) {
	g_cmdline = cmdline;
	g_hinst = h_inst;
	
#ifdef PROGRAM_CLIENT
	//int *Poopy = (int*)Memory::AllocMem(sizeof(int));

	return ClientEntryPoint();
#endif
	
	/*
	int *Poopy = Memory::Alloc<int>();
	Video::Init( h_inst, 640, 480, 32, 0 );
	Video::Shaders::ObjectShader *objshader = new Video::Shaders::ObjectShader( "objshader" );
	
	
	Video::SetCullingMode( Video::CULLMODE_NONE );
	Video::SetBlendMode( Video::BLEND_ADD );
	test_texture = new Video::Texture( "moontest" );
	test_texture->LoadFile( "materials/moontest.png" );
	test_vb = new Video::VertexBuffer( Video::VF_GENERIC );

	Video::generic_vertex data[4];
	data[0] = Video::SetupGenericVertex( 0.0f, 0.0f, 0.0f, 255, 255, 255, 255, 0.0, 0.0 );
	data[1] = Video::SetupGenericVertex( 1.0f, 0.0f, 0.0f, 255, 255, 255, 255, 1.0, 0.0 );
	data[2] = Video::SetupGenericVertex( 1.0f, 1.0f, 0.0f, 255, 255, 255, 255, 1.0, 1.0 );
	data[3] = Video::SetupGenericVertex( 0.0f, 1.0f, 0.0f, 255, 255, 255, 255, 0.0, 1.0 );
	
	test_vb->BufferData( data, 4 * sizeof(Video::generic_vertex), GL_STATIC_DRAW_ARB );
	
	Video::RunWindowLoop( &WindowLoopFrame );
	
	delete test_vb;
	delete test_texture;

	Video::Unload();

	*/

	/*
	Textures::LoadDefault();
	Scene::Init();
//	sound::init();
	Input::Init();
	Models::Init();
	rnd::setup_table();

	printf(" \n ---- TEST 1 ---- \n" );
	
	//objects::debug1(poody);
	
	printf(" \n ---- TEST 1 ---- \n" );
	 
	Startup();
	
	Video::Unload();
	Input::Unload();
//	sound::unload();
	
	//WSACleanup();
	*/
//	game_engine = new Game::Engine();
	return 0;
}

//-------------------------------------------------------------------------------------------------
INT WINAPI WinMain( HINSTANCE hinst, HINSTANCE p1, LPSTR p2, INT p3 ) {
	
	WinMainREAL( hinst, p1,p2,p3);

	printf( "DEBUG: Program exited, press enter to burn to death\n" );
	getc(stdin);
	return 0;
}

//-------------------------------------------------------------------------------------------------
int main( int argc, char *argv[] ) {
	g_argc = argc;
	g_argv = argv;
	ServerEntryPoint();
	return 0;
}
