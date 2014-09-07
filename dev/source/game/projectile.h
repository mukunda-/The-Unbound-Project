//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//
#pragma once

#ifndef PROJECTILE_H
#define PROJECTILE_H

namespace objects {

class projectile : public entity {

	cml::vector3f vel;
	cml::vector3f dir;

	int explosion_size;

	float fuse;
	float launch;
//	ALuint poopy ;
	worldrender::lightInstance2 *light;

	Audio::Channel *ac;
public:
	void update();
	void render();

	void set_attributes( int explosion );
	void start( cml::vector3f position, cml::vector3f direction );
	
	//int poops;
};

}

#endif
