//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//
#include "stdafx.h"

#if 0

	Models::Model poody;
	

extern int debug_instance_count ;

namespace Ingame {


Players::Player test;

planet::context ct_test;

worldrender::lightInstance2 *testlight;
worldrender::lightInstance2 *testlight2;

Audio::Sample test_sample;
Audio::Channel *test_ch;
//Audio::Sound test_sound;

void Start () {
	printf("starting game\n");
	
	Audio::Initialize();
	test_sample.CreateFromWAV( "sound\\test1.wav" );
	
	test.reset( 64*16, 650, (64)*16 );
	/*
	test_ch = Audio::CreateChannel( false );
	test_ch->SetSource( &test_sample) ;
	test_ch->SetVolume( 2.0 );
	test_ch->SetSamplingRate( 1.0, false );
	test_ch->Set3DPosition( cml::vector3d(0,0,0) );
	test_ch->Start();
	test_ch->Add();

	*/
	ct_test.initialize( "worlds\\test1.world", NULL );
	planet::set_context(&ct_test);
	worldrender::set_context( &ct_test );
	
	MSG msg;
	ZeroMemory( &msg, sizeof( msg ) );

	gametime::reset();

	testlight = worldrender::light_create( 20.0f, 1.0f, 0.5f, 0.1f );
	//testlight2 = worldrender::light_create( 15, 255,255,255);

	//poody.Load( "D:\\dev\\jpcleanup\\blender\\pood.fbx" );
	//poody.UpdateVertexBuffer( true );

	float pooda = 0.0f;

	while( msg.message != WM_QUIT )
    {
        if( PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) )
        {
            TranslateMessage( &msg );
            DispatchMessage( &msg );
        } else {
			
            //Scene::Render();
			//Video::swap();


			Input::Begin();

			//network::process();

			int move=0;
			if( Input::KeyPressed( DIK_W ) ) move |= Players::MOVE_FORWARD;
			if( Input::KeyPressed( DIK_S ) ) move |= Players::MOVE_BACK;
			
		
				if( Input::KeyPressed( DIK_A )  ) move |= Players::MOVE_LEFT;
				if( Input::KeyPressed( DIK_D )  ) move |= Players::MOVE_RIGHT;
		
			if( Input::KeyPressed( DIK_X ) ) move |= Players::MOVE_DOWN;
			if( Input::KeyPressed( DIK_SPACE ) ) 
				move |= Players::MOVE_UP;
			
			
			hud::set_mode(Input::KeyPressed(1) ? hud::MODE_INVENTORY : hud::MODE_LIVE);

//			if( Input::MouseClicked( 1) ) {
	//			hud::set_mouse_mode( !true );
		//	} else if( Input::MouseUp(1) ) {
			//	hud::set_mouse_mode( !false );
		//	}

			if( Input::KeyPressed( DIK_F ) ) {
//				planet::destroy_radius( (int)test.pos[0], (int)test.pos[1], (int)test.pos[2], 15 );
			}
			//if( Input::MouseClicked(0) ) {
				
				
				cml::vector3i pood;cml::vector3i pood2;
				cml::vector3f ray = Video::GetPickRay( cml::vector2f((float)Video::SCREEN_WIDTH/2.0f,Video::SCREEN_HEIGHT/2.0f) );
				ray *= 512.0;
				int cell;
				if( Input::MouseClicked(0) ) {
					planet::pick( Video::GetCamera(), Video::GetCamera() + ray, NULL, NULL, &pood, NULL );
					cell = planet::CELL_LIGHTBLOCK+1;//LIGHTBLOCK + (planet::random_number()  % 4);
				} else {
					planet::pick( Video::GetCamera(), Video::GetCamera() + ray, NULL, &pood, NULL, NULL );
					cell = planet::CELL_AIR;
				}
				//planet::destroy_radius( pood[0], pood[1], pood[2], 10 );//
				//planet::set_cell( pood, cell );

				planet::pick( Video::GetCamera(), Video::GetCamera() + ray, NULL, NULL, &pood, NULL );
				//worldrender::light_setpos(testlight2,pood[0],pood[1],pood[2]);
			//}

			{
				
				worldrender::light_setpos(testlight, (float)test.pos[0], (float)test.pos[1]+1.7f, (float)test.pos[2]);
				
			}
			//if( !hud::get_mouse_mode() ) {
				test.look( (float)Input::MouseRX() * 0.005 , Input::MouseRY() * 0.005  , true );
			//} else {
				//if( Input::KeyPressed( DIK_A ) )
				//	test.look( -0.03 * gametime::frames_passed()  , 0, true );
				//else if( Input::KeyPressed( DIK_D ) )
				//	test.look( 0.03  * gametime::frames_passed() , 0, true );
			//}

			if( Input::MouseClicked(0) || pooda >= 1.0f ) {
				if( pooda >= 1.0f ) pooda = 0.0f;
				//for( int i = 0; i < 10; i++ ) {
					objects::projectile *np;
					//if( flicker ) {
					np = new objects::projectile;
					np->set_attributes(10);

					cml::vector3f source,target;
					source = Video::GetRelativeCameraPoint( 1.0, 0.5, 0.0 );
					target = Video::GetRelativeCameraPoint( 0.5,0.5,10.0 );//0.25+rnd::next_float() * 0.50,0.25+rnd::next_float() * 0.50, 10.0 );

					//source = Video::GetRelativeCameraPoint( 0.5, 0.0, 20.0 ) + cml::vector3f((rnd::next_float() - 0.5) * 40, (rnd::next_float() - 0.5) * 40, (rnd::next_float() - 0.5) * 40);
					///zzzz//////source = cml::vector3f( Video::GetCamera()[0] - 50 + rnd::next_float() * 100.0f, Video::GetCamera()[1] + 100.0f, Video::GetCamera()[2] - 50 + rnd::next_float() * 100.0f );
					//target = Video::GetCamera() + cml::vector3f( (rnd::next_float() - 0.5) * 60.0f, (rnd::next_float() - 0.5) * 60.0f, (rnd::next_float() - 0.5) * 60.0f );
					np->start( source, target-source );
	//					(Video::nearPlane[2] + Video::nearPlane[3] ) / 2.0 + (Video::nearPlane[3] - Video::nearPlane[0]) * 20.0, 
		//				cml::normalize(
			//				((Video::farPlane[0] + Video::farPlane[1] + Video::farPlane[2] + Video::farPlane[3]) / 4.0)-
				//			((Video::nearPlane[0] + Video::nearPlane[1] + Video::nearPlane[2] + Video::nearPlane[3]) / 4.0) ));
				
					objects::add( np );
				//}
			}
			if( Input::MousePressed(0) ) {
				pooda += 0.20f * (float)gametime::frames_passed();
			} else {
				pooda = -0.5f;
			}
			/*
			if( Input::MouseClicked(1) ) {
				Video::ShakeCamera( 0.7f );
			}*/

			Input::End();
			screenshader::UpdateScreenFlash();

			//particles::add( Video::GetRelativeCameraPoint( 0.5, 0.5, 15 ), particles::PK_SMOKEY );
			//particles::add( test.pos + cml::vector3f( -50 + rnd::next_float() * 100, 20, -50 + rnd::next_float() * 100 ), particles::PK_RAINDROP );
			gametime::newTick();
			test.move( move );
			test.sprint(Input::KeyPressed( DIK_LSHIFT ));
			test.tick();
			objects::update();
			particles::update();
			hud::update ();
			clouds::update();
			//map3::setposition((int)floor(test.pos.x/8.0), (int)floor(test.pos.y/8.0), (int)floor(test.pos.z/8.0) );
			planet::process_mods();
			
			
			//poody.SetFrame( 1 );

			cml::vector3f pooder;
			
			//worldrender::light_setpos( poop2, test.pos.x, test.pos.y, test.pos.z );
			//Shaders::MS.Use();
			
			Video::addCameraShakeTime( (float)gametime::frames_passed() * 0.8f );
			
			Video::fadeCameraShake( 0.91f );
			//Video::getCameraShake();
			Video::SetCameraBob( test.getHeadBob(), test.getHeadBobScale() );
			Video::SetCamera( (float)test.pos[0], (float)test.pos[1]+1.7f, (float)test.pos[2], (float)test.angle, (float)test.pitch );
			Audio::SetListener2( Video::GetCamera(), Video::GetCameraForwardVector(), Video::GetCameraRightVector(), 21.0 / 100.0 / 2.0 );

//			sound::update_position();
			graphics::compute_viewing_planes();
			graphics::new_scene();
			worldrender::update();
			

			hud::draw();
			objects::draw();
			particles::draw();
		//	objects::load_vertex_data();
			

			
			//poody.UpdateVertexBuffer(false);
			
		
			
			 
			/*
			fps_counter++;
			int read_clock = clock();
			if( read_clock >= (fps_time+1000) ) {
				fps_saved = fps_counter;
				fps_counter=0;
				fps_time = read_clock;
				
			}*/

			//debug_instance_count = (u32)planet::get_chunk_direct((int)test.pos.x/16, (int)test.pos.y/16, (int)test.pos.z/16);
			
			char debug_text[512];
			//sprintf( debug_text, "FPS: %i", fps_saved );
			//hud::draw_text( 5, 60, debug_text );
			//sprintf( debug_text, "debug1: %i", debug_instance_count );
			//hud::draw_text( 5, 90, debug_text );
			sprintf( debug_text, "X: %i:%i, Y: %i:%i, Z: %i:%i", (int)test.pos[0]/16, (int)test.pos[0]&15, (int)test.pos[1]/16, (int)test.pos[1]&15, (int)test.pos[2]/16, (int)test.pos[2]&15 );
			hud::draw_text( 5, 120, debug_text );
			sprintf( debug_text, "active chunks: %i/%i",debug_instance_count,8000 );
			hud::draw_text( 5, 150, debug_text );

			
			Scene::Render();
			

			Video::swap();

			if( Input::KeyPressed( DIK_ESCAPE ) ) {
				PostQuitMessage(0);
			}
		}
    }
}

} // namespace

#endif
