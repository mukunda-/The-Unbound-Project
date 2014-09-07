//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//

// audio playback header
// 

#pragma once

#ifndef AUDIO_H
#define AUDIO_H

#include "audio/Channel.h"
#include "audio/Sample.h"

//-----------------------------------------------------------------------------
namespace Audio {
	
//-----------------------------------------------------------------------------
typedef struct t_AudioGlobalSettings {
	cml::vector3d listener_position;
	double listener_separation;

	cml::vector3d listener_forward;
	cml::vector3d listener_right;
	cml::vector3d listener_up;
	double volume;

} AudioGlobalSettings;

void StartSimple3D( const Sample *source, cml::vector3d position, float rate = 1.0 );
void StartSimple3D( int sample_index, cml::vector3d position, float rate = 1.0 );

void SetListener2( cml::vector3d center, cml::vector3d forward, cml::vector3d right, double separation );

void Initialize();
void InitializeDevice();
void Unload();

void SetMasterVolume( float master_volume );

void Panic();

}


#endif
