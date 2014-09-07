//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//

#include "stdafx.h"
#include "include_game.h"
#include "video/shaders/uishader.h"
#include "graphics/fontmaterial.h"

static const char BUILD_DATE[] = __DATE__ "/" __TIME__;

Game::Game() {
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

static float deg2rad( float degrees ) {
	return degrees / 180.0f * 3.14159265f;
}
static float rad2deg( float radians ) {
	return radians * 180.0f / 3.14159265f;
}

static const float pi = 3.14159265f;

class GameCamera {

public:
	Video::Camera camera;
	Eigen::Vector3f orientation;

	EIGEN_MAKE_ALIGNED_OPERATOR_NEW

	GameCamera() {
		orientation = Eigen::Vector3f( 0.0f, 1.0f, 0.0f );
	}



	void UpdateVideo() {
		camera.Fixup();
		camera.UpdateVideo();
	}

	void SetPosition( const Eigen::Vector3f &pos ) {
		camera.SetPosition( pos );
	}

	void LookAt( const Eigen::Vector3f &pos ) {
		camera.LookAt( pos, orientation );
	}

	void SetOrientation( const Eigen::Vector3f &up ) {
		orientation = up;
	}

	void Pan( float mx, float my ) {
		
		if( my < 0.0f ) {
			float ang = my * 0.001f;
			Eigen::Vector3f fwd = orientation.cross(camera.Right()).normalized();
			float theta = acos(fwd.dot(camera.Forward()));
			camera.Rotate( -ang );
			float theta2 = acos(fwd.dot(camera.Forward()));
			if( theta2 > theta ) {
				// we are moving away from the center
				if( theta2 > deg2rad(95.0f) ) {
					// we went too far
					camera.Rotate( -(theta2 - deg2rad(95.0f)) );
				}
			} 
		} else if( my > 0.0f ) {
			float ang = -my * 0.001f;
			Eigen::Vector3f fwd = orientation.cross(camera.Right()).normalized();
			float theta = acos(fwd.dot(camera.Forward()));
			camera.Rotate( ang );
			float theta2 = acos(fwd.dot(camera.Forward()));
			if( theta2 > theta ) {
				// we are moving away from the center
				if( theta2 > deg2rad(95.0f) ) {
					// we went too far
					camera.Rotate( (theta2 - deg2rad(95.0f)) );
				}
			} 
		}
		camera.Rotate( 0.0f, -mx * 0.001f );
	}

	// 
	void MoveRel( const Eigen::Vector3f &amt ) {
		if( amt[0] != 0.0f ) {
			camera.Move( camera.Right() * amt[0] );
		}
		if( amt[1] != 0.0f ) {
			camera.Move( camera.Up() * amt[1] );
		}
		if( amt[2] != 0.0f ) {
			camera.Move( camera.Forward() * amt[2] );
		}
	}

	void Rotate( const Eigen::Vector3f &angles ) {
		camera.Rotate(angles);
	}

	void OnTick() {
		Eigen::Vector3f oriented_up = orientation.cross(camera.Forward()).normalized();
		Eigen::AngleAxisf a( 3.14159f/2.0f, camera.Forward());
		float roll = -oriented_up.dot(camera.Up());
		oriented_up = a * oriented_up;
		float tolerance = orientation.dot(camera.Forward());
		float balance = acos(oriented_up.dot(camera.Up()));

		float horizon = abs(pi/2.0f - (acos(tolerance)));

		float deadzone = deg2rad( 80.0f );
		float strictzone = deg2rad( 5.0f );
 
		float speed = (horizon - strictzone) / (deadzone-strictzone);// * 180.0f;
		if( speed < 0.0f ) speed = 0.0f;
		if( speed > 1.0f ) speed = 1.0f;
		speed = 1.0f - speed; 
		speed = 1.0f - pow((1.0f-speed),0.3f);
		speed *= 0.3f; 
				
		float amount = balance; 
				
		if( amount > 0.0f ) {
			 
			if( roll < 0.0f ) {
				amount = -amount;
			}
			camera.Rotate( 0.0f, 0.0f, amount * speed ); 
		}
	}
};

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
		Video::BLEND_OPAQUE, mymat, 0, 0, 0, 44, GL_LINES );
	

	Graphics::AddElement( test_element );

	GameCamera cam;
	cam.SetPosition( Eigen::Vector3f( 0, 0, 5.0 ) );
	cam.LookAt( Eigen::Vector3f( 0,0,0 ) );
	cam.UpdateVideo();
	  
	float angle = 0.0;


	SDL_SetRelativeMouseMode(SDL_TRUE);
	/*
	Graphics::Element test_element2;
	Graphics::SetupElement( test_element2, GenerateTestGeometry2(), 
		Video::BLEND_OPAQUE, mymat2, 0, 0, 0, 6, GL_TRIANGLES );
	test_element2.m_layer = Graphics::LAYER_UI;
	Graphics::AddElement( test_element2 );
	*/
	 
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
