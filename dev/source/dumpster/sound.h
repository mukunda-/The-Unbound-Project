//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//

#if 0
#ifndef SOUND_H
#define SOUND_H

#include <al.h>

namespace sound {


enum {
	EXPLOSION1,
	EXPLOSION2,
	EXPLOSION3,
	ROCKET1,
	BLASTWAV
};

void init();
void unload();
void update_position();


ALuint create( int sound );
void play( ALuint handle );
void move( ALuint handle, float x, float y, float z ); // (todo doppler)
void remove( ALuint handle );

}

#endif

#endif
