//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//

// openal sound extension
#include "stdafx.h"

#if 0

#include <Windows.h>
#include <stdio.h>
#include <al.h>
#include <AL/alut.h>

#include "video.h"

char *sound_list[] = {
	"sound\\exp1.wav",
	"sound\\exp2.wav",
	"sound\\exp3.wav",
	"sound\\javelin1.wav",
	"sound\\javelin1.wav",
	0
};

float gain_list[] = {
	1.0,
	1.0,
	1.0,
	0.4,
};

namespace sound {

//ALCdevice *Device;
//ALCcontext *Context;

ALuint *buffers;
int num_buffers;

void get_num_buffers() {
	for( num_buffers = 0; sound_list[num_buffers]; ) num_buffers++;
	buffers = new ALuint[num_buffers];
}

void init() {

	get_num_buffers(); 

	// Init openAL
alutInit(0, NULL);
	// Clear Error Code (so we can catch any new errors)
alGetError();

//	Device = alcOpenDevice(NULL);
	//
//	if( Device ) {
//		Context = alcCreateContext(Device, NULL);
//		alcMakeContextCurrent(Context);
//	}

//	alGetError();
	int error;
	for( int i = 0; i < num_buffers; i++ ) {
	buffers[i] = alutCreateBufferFromFile( sound_list[i] );
	}

	//ALuint sourcey;
	// Generate the sources
	//alGenSources(1, &sourcey);
	//if ((error = alGetError()) != AL_NO_ERROR)
	//{
	//  printf("alGenSources : %d", error);
	//  return;
	//}

	//alSourcei(sourcey, AL_BUFFER, buffers[0]);

	//alSourcePlay(sourcey);
	
alListenerf( AL_GAIN, 20.0 );
	
}

void update_position() {
	float pos[3], vel[3], orientation[6];

	pos[0] = Video::GetCamera()[0];
	pos[1] = Video::GetCamera()[1];
	pos[2] = Video::GetCamera()[2];

	cml::vector3f upvec = Video::nearPlane[0] - Video::nearPlane[1];
	cml::vector3f fvec = (Video::farPlane[0] + Video::farPlane[2])/2.0 - (Video::nearPlane[0] + Video::nearPlane[2])/2.0;
	upvec.normalize();
	fvec.normalize();

	orientation[0] = fvec[0];
	orientation[1] = fvec[1];
	orientation[2] = fvec[2];

	orientation[3] = upvec[0];
	orientation[4] = upvec[1];
	orientation[5] = upvec[2];

	vel[0] = vel[1] = vel[2] = 0;

	alListenerfv( AL_POSITION, pos );
	alListenerfv( AL_VELOCITY, vel );
	alListenerfv( AL_ORIENTATION, orientation );
}

void unload() {
	//alDeleteSources(NUM_SOURCES, source);
	alDeleteBuffers(num_buffers, buffers);

	alutExit();
}

//-------------------------------------------------------------------------------------

ALuint create( int sound ) {
	ALuint handle;
	alGenSources( 1, &handle );
	alSourcei( handle, AL_BUFFER, buffers[sound] );
	alSourcef( handle, AL_GAIN, gain_list[sound] );
	return handle;
}

void play( ALuint handle ) {
	alSourcePlay( handle );
}

void move( ALuint handle, float x, float y, float z ) {
	float pos[3];
	pos[0] = x; pos[1] = y; pos[2] = z;
	alSourcefv( handle, AL_POSITION, pos );
}

void remove( ALuint handle ) {
	alDeleteSources( 1, &handle );
}

}

#endif
