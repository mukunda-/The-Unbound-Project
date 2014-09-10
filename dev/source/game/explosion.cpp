//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//

#include "stdafx.h"

#if 0 // obsolete

namespace objects {

explosion::explosion( cml::vector3f position, float p_radius ) {
	pos = position;
	radius = p_radius;
	animation = 0.0;
	timer1 = 1.0;
	light = worldrender::light_create(15.0f,2.0f,2.0f,1.95f);
	dead=false;
	//snd = sound::create(randint(sound::EXPLOSION1,sound::EXPLOSION3));
//	sound::move( snd, position[0], position[1], position[2] );

	blastwave = 0;
	hit_camera=false;
}

explosion::~explosion() {
	//sound::remove( snd );
}

void explosion::update() {
	float time = (float)gametime::frames_passed();
	if( !dead ) {
		if( !hit_camera ) {
			blastwave += time * 5.6715f; // speed of sound
			if( Video::GetCameraDistance2(pos) < blastwave*blastwave ) {
				hit_camera=true;
				Video::ShakeCamera( 0.8f / (1.0f+(blastwave/32.0f)) );
				
			}
			if( blastwave > 100 ) {
				hit_camera=true;
			}
		}
		if( animation == 0.0 ) {
			Audio::StartSimple3D( randint(Audio::S_EXPLOSION1,Audio::S_EXPLOSION3), pos,  1.0f + (rnd::next_float()-0.5f)*0.4f );

			float dist = Video::GetCameraDistance( pos );
			if( dist < 50 ) {
				screenshader::SetScreenFlash( 32.0f / (1.0f+(dist/16.0f)) );
			}
		}
		if( timer1 > 0.0 ) {
			timer1 -= 0.3f*time;
			if( !(timer1 > 0.0) ) {
				
				particles::add( particles::PK_BIGSMOKEY, pos );
				particles::add( particles::PK_BIGSMOKEY, pos );
				particles::add( particles::PK_BIGSMOKEY, pos );
				particles::add( particles::PK_BIGSMOKEY, pos );
				particles::add( particles::PK_BIGSMOKEY, pos );
				particles::add( particles::PK_BIGSMOKEY, pos );
				particles::add( particles::PK_BIGSMOKEY, pos );
				particles::add( particles::PK_BIGSMOKEY, pos );
				particles::add( particles::PK_BIGSMOKEY, pos );
				particles::add( particles::PK_BIGSMOKEY, pos );
				particles::add( particles::PK_BIGSMOKEY, pos );

				
				particles::add( particles::PK_BIGBLACKSMOKEY, pos );
				particles::add( particles::PK_BIGBLACKSMOKEY, pos );
				particles::add( particles::PK_BIGBLACKSMOKEY, pos );
				particles::add( particles::PK_BIGBLACKSMOKEY, pos );
				particles::add( particles::PK_BIGBLACKSMOKEY, pos );
				particles::add( particles::PK_BIGBLACKSMOKEY, pos );
				particles::add( particles::PK_BIGBLACKSMOKEY, pos );
				particles::add( particles::PK_BIGBLACKSMOKEY, pos );
				particles::add( particles::PK_BIGBLACKSMOKEY, pos );
				particles::add( particles::PK_BIGBLACKSMOKEY, pos );
				particles::add( particles::PK_BIGBLACKSMOKEY, pos );
				
				/*
				particles::add( particles::PK_FLARES1, pos );
				particles::add( particles::PK_FLARES1, pos );
				particles::add( particles::PK_FLARES1, pos );
				particles::add( particles::PK_FLARES1, pos );
				particles::add( particles::PK_FLARES1, pos );
				particles::add( particles::PK_FLARES1, pos );
				particles::add( particles::PK_FLARES1, pos );
				particles::add( particles::PK_FLARES1, pos );*/

		//		planet::destroy_radius( pos[0], pos[1], pos[2], radius );		TODO: EXPLOSION, SERVERSIDE STUFF
				planet::destroy_radius( (int)pos[0], (int)pos[1], (int)pos[2], (int)radius );
			}
		}
		worldrender::light_setpos(light,pos[0],pos[1],pos[2]);
		//light->brightness = ((float)animation * 5.0f );
		 
		float a = (float)(int)(20.0f - (animation / 16.0f) * 20.0f);
		if( a < 0.0f ) a = 0.0f;
		light->brightness = a; 
	
		animation += 0.25f*time;
		if( animation >= 16.0 ) {
			dead = true;
			worldrender::light_destroy(light);
		}
	}
}

void explosion::render() {
	int frame = (int)floor(animation);
	float u,v;
	u = (float)(frame % 4) * 64.0f/1024.0f + 0.0f/1024.0f;
	v = (float)(frame / 4) * 64.0f/1024.0f + 512.0f/1024.0f;
	
	graphics::draw_sprite( pos, radius*4.0f, radius*4.0f, u, v, u+64.0f/1024.0f, v+64.0f/1024.0f, Video::BLEND_ADD, 255, 255, 255, 255, 0,false );
	u += 256.0f/1024.0f;
	graphics::draw_sprite( pos, radius*4.0f, radius*4.0f, u, v, u+64.0f/1024.0f, v+64.0f/1024.0f, Video::BLEND_ADD, 255, 255, 255, 255, 0,false );
	
}

}


#endif