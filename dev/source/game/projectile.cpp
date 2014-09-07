//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//
#include "stdafx.h"

namespace objects {

void projectile::set_attributes( int explosion ) {
	explosion_size = explosion;
}

void projectile::start( cml::vector3f position, cml::vector3f direction ) {
	dir = normalize(direction);
	pos = position;
	vel = dir *5.0;//2.5 * 0.5;
	light = worldrender::light_create(15.0f, 1.0f,1.0f,0.95f);
	fuse = 0.1f;
	launch = 1.0f;
	dead=false;

	Audio::StartSimple3D( Audio::S_ROCKETFIRE, pos );

	ac = Audio::CreateChannel(false);
	ac->SetSource( Audio::SampleFromTable( Audio::S_ROCKETTRAIL ) );
	ac->SetVolume(1.0);
	ac->Set3DPosition( pos );
	ac->SetSamplingRate(  1.0 + (rnd::next_float()-0.5)*0.1 , false );
	ac->Start();
	Audio::AddChannel( ac) ;
	

}
int random_lerp( int low, int high ) {
	return low + (((high-low) * planet::random_number())>>8);
}

void projectile::update() {

	

	for( int time = gametime::frames_passed(); time; time-- ) {
		if( dead ) return;

		ac->Set3DPosition( pos );
		//sound::move(poopy,Video::GetCamera()[0], Video::GetCamera()[1], Video::GetCamera()[2]);//poopy,pos[0],pos[1],pos[2]);

		cml::vector3i ib;
		
		//if( launch > 1.0f ) {
			particles::add( particles::PK_SMOKEY, pos, vel*0.25 );
		//} else {
		//	particles::add( particles::PK_BIGSMOKEY, pos, vel*0.25 );
		//}
		//particles::add( particles::PK_GRASS_PARTICLES, pos, vel*0.25 );
		//particles::add( particles::PK_FIRE, pos );
//		particles::add( pos, cml::vector3f(0,0,0), 0.0, 0, 0, 1, 0.02, 1.0, -0.03, true, 128.0/512.0, 128.0/512.0,64.0/512.0);
		
		if( planet::pick( pos, pos+vel, NULL, &ib, NULL, NULL ) ) {
			
			objects::add( new explosion( cml::vector3f(ib) + cml::vector3f(0.5f,0.5f,0.5f),7 ) );
			//planet::destroy_radius( ib[0], ib[1], ib[2], 10 );
			dead = true;
			worldrender::light_destroy(light);
			//sound::remove(poopy);
			
			ac->StopAndDelete();
			// explode
		} else {
			pos += vel;
			vel[1] -= 0.01f * time;
			/*
			if( launch < 1.0f ) {
				launch += 0.1f;
			} else {
				vel += dir * 0.1;
			}*/
			
			if( fuse < 1.0 ) {
				fuse += 0.005f * time;
				if( fuse > 1.0f ) {
					fuse = 1.0f;

					objects::add( new explosion( pos, 7 ) );
					//planet::destroy_radius( ib[0], ib[1], ib[2], 10 );
					dead = true;
					worldrender::light_destroy(light);

					
					ac->StopAndDelete();
					return;
				}
				//vel *= pow( 0.9f, time );
			}/* else {
				if( vel.length_squared() < 3.0 * 3.0 ) {
					vel += cml::normalize(vel) * 0.05 * time;
					if( vel.length_squared() > 3.0 * 3.0 )
						vel = cml::normalize(vel) * 3.1;
				}
			}*/
			worldrender::light_setpos( light, pos[0], pos[1], pos[2] );
			
			//sound::move(poopy,pos[0],pos[1],pos[2]-1);
		}
	}
}

void projectile::render() {
	//if( dead ) return;
	cml::vector3f nv = vel;
	nv.normalize();
	graphics::draw_sprite( pos,0.4f, 0.4f, 961.0f/1024.0f, 1.0f/1024.0f, (961.0f+2.0f)/1024.0f, (1.0f+2.0f)/1024.0f, Video::BLEND_OPAQUE, 255,255,255,255,0, false );
	graphics::draw_sprite( pos-nv*1.0,8.0f, 8.0f, 641.0f/1024.0f, 1.0f/1024.0f, (641.0f+254.0f)/1024.0f, (1.0f+254.0f)/1024.0f, Video::BLEND_ADD, 255,255,255,255,0, false );
	
 //	objects::draw_billboard_solid( pos, 0.3, 128.0/512.0, 112.0/512.0, (128.0+8.0)/512.0, (112.0+8.0)/512.0, 255, 0 );
}

}
