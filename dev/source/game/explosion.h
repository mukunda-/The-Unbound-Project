//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//
#pragma once

#ifndef EXPLOSION_H
#define EXPLOSION_H

namespace objects {

class explosion : public entity {

	float radius;
	float animation;
	float timer1;
	
	float blastwave;
	bool hit_camera;

	worldrender::lightInstance2 * light;

	//ALuint snd;
public:

	explosion( cml::vector3f position, float p_radius );
	~explosion();
	void update();
	void render();

};

}

#endif
