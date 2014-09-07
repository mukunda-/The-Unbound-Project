//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//
//------------------------------------------------------------------------------------------------------
#include "stdafx.h"
 

struct myvertex {
	float x,y,z;
	float u,v;
};

class myshader : public Video::Shader, public Shaders::ShaderCamera<myshader> {

	friend class Shaders::ShaderCamera<myshader>;

	int u_sampler;
	GLint a_position;
	GLint a_uv;
	int u_translation;

public:
	myshader() : Shader( "texturetest" ) {

		AddUniform( u_sampler, "sampler" );
		AddUniform( u_translation, "translation" );
		AddAttribute( a_position, "position" );
		AddAttribute( a_uv, "uv" );
		
		 
		AddShader( "glsl\\texturetest.v.glsl", GL_VERTEX_SHADER );
		AddShader( "glsl\\texturetest.f.glsl", GL_FRAGMENT_SHADER );
		Link();
		Register();
	} 

	void SetVertexAttributePointers( int offset, int set ) override {
		glVertexAttribPointer( a_position, 3, GL_FLOAT, GL_FALSE, 20, (void*)offset );
		glVertexAttribPointer( a_uv, 2, GL_FLOAT, GL_FALSE, 20, (void*)(offset+12) );

	}

	void SetTranslation( float x, float y, float z ) {
		glUniform3f( u_translation, x, y, z );
	}

	void SetSampler( int sampler ) {
		glUniform1i( u_sampler, sampler );
	}
};

//------------------------------------------------------------------------------------------------------
INT WINAPI WinMain( HINSTANCE hinst, HINSTANCE p1, LPSTR p2, INT p3 ) {
	 
	
	if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_EVENTS) != 0){
		std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
		return 0;
	}
	
	Video::Open( 1200, 675 ); 

	int texture_units;
		
	glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &texture_units);
	std::cout << "texture units: " << texture_units << std::endl;

#define NTEX 16
#define NDRAW 32
	 
	GLuint texhandles[NTEX];
	glGenTextures( NTEX, texhandles );

	uint8_t *my_textures[NTEX];//[256*256*3];
	for( int i = 0; i < NTEX; i++) {
		my_textures[i] = new uint8_t[256*256*3];

		uint8_t *tex = my_textures[i];
		for( int x = 0; x < 256; x++ ) {
			for( int y = 0; y < 256; y++ ) {

				tex[(x+y*256)*3] = rand()%256;
				tex[(x+y*256)*3+1] = i*16;//rand()%256;
				tex[(x+y*256)*3 +2 ] = rand()%256;
				
			}
		}

		glBindTexture( GL_TEXTURE_2D, texhandles[i] );
		glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, 256, 256, 0, GL_RGB, GL_UNSIGNED_BYTE, tex );

		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	}

	

	Video::SetDepthBufferMode( Video::ZBUFFER_DISABLED );


	//for( int i = 0; i <

	myvertex test[6] = 
		{ 
			{0,1,0,0,0},
			{0,0,0,0,1},
			{1,0,0,1,1},

			{1,0,0,1,1},
			{1,1,0,1,0},
			{0,1,0,0,0}
		};
	//test[0] = {0,0,0,1,1};

	Graphics::Vertex::Generic3D verts[22*2];
	//Graphics::Vertex::Generic3D verts[3];
 
	Shaders::LineTester shader;
	
	for( int i = 0; i < 11; i++ ) {
		verts[i*4+0] = Graphics::Vertex::Generic3D( -10.0f + (float)i*2, 0.0f, -10.0f );
		verts[i*4+1] = verts[i*4+0];
		verts[i*4+1].z = 10.0f;

		verts[i*4+2] = Graphics::Vertex::Generic3D( -10.0f, 0.0f, -10.0f + (float)i*2 );
		verts[i*4+3] = verts[i*4+2];
		verts[i*4+3].x = 10.0f; 
	}
	

	Video::VertexBuffer buffer;
	buffer.Load( &verts, sizeof verts, GL_STATIC_DRAW );

	shader.Use(); 

	Video::SetBackgroundColor( 90/255.0,179/255.0,212/255.0  );
	Video::Camera camera;
	//camera.SetPosition( Eigen::Vector3f( 3,2 , 5 ) );
	camera.SetPosition( Eigen::Vector3f( 0.0,3.0 , 9.0 ) );
	camera.LookAt( Eigen::Vector3f( 0,0,0 ), Eigen::Vector3f(0,1,0) );
	camera.UpdateVideo();
	shader.SetCamera();

	myshader shader2;
	shader2.Use();
	shader2.SetCamera();
	Video::VertexBuffer testbuffer;
	testbuffer.Load( &test, sizeof test, GL_STATIC_DRAW );

#define TEST_MODE 0
	
	shader2.Use();

#if TEST_MODE == 0
	glBindTexture( GL_TEXTURE_2D, texhandles[0] );
	shader2.SetSampler( 0 );

#else
	for( int i = 0; i < NTEX;i++ ) {
		glActiveTexture( GL_TEXTURE0+i );
		glBindTexture( GL_TEXTURE_2D, texhandles[i] );
	}
#endif
	 
	SDL_Event e;
	bool quit=false;
	do {
		while (SDL_PollEvent(&e)){
			if (e.type == SDL_QUIT) {
				quit = true;
			} else if( e.type == SDL_KEYDOWN ) {
				if( e.key.keysym.scancode == SDL_SCANCODE_ESCAPE ) {
					quit = true;
				}
			}
			
		}
		 
		Video::Clear();   
		shader.Use();
		buffer.Render( GL_LINES, 0, 0, 44 );
		shader2.Use();
		for( int i = 0; i < 5000; i++ ) {
			float x, y, z;

#if TEST_MODE == 0
			glBindTexture( GL_TEXTURE_2D, texhandles[i%NTEX] );
#else
			shader2.SetSampler( i%NTEX );
#endif
			x = ((float)rand() / (float)RAND_MAX) * 10.0 - 5.0;
			y = ((float)rand() / (float)RAND_MAX) * 8.0 - 5.0;
			z = ((float)rand() / (float)RAND_MAX) * 32.0 - 32.0;
			shader2.SetTranslation( x-0.5, y-0.5, z );
			testbuffer.Render( GL_TRIANGLES, 0, 0, 6 );
		}
		
		Video::Swap();

	} while(!quit);
 
	Video::Close();
	SDL_Quit();
	return 0;
}
