//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//
#pragma once

//-------------------------------------------------------------------------------------------------
#ifndef AUDIO_CHANNEL_H
#define AUDIO_CHANNEL_H

#include "audio/Sample.h"

//-------------------------------------------------------------------------------------------------
namespace Audio {


//-----------------------------------------------------------------------------
typedef struct t_ChannelState {
//-----------------------------------------------------------------------------
	// the flags determine which entries in this struct are valid to
	// control/change the properties of the channel
	int flags; // FL_*

	// change the sample source (and stop the sound)
	// FL_SOURCE
	// combine with FL_START to start the sound
	const Sample *source;		

	// set/change sampling rate
	// use with FL_RATE
	double sampling_rate;		
	bool sampling_rate_direct;	// true: r=sampling_rate, false: r=sampling_rate*source.rate

	// set time offset
	// use with FL_TIME
	// TODO: realtime vs time??
	double time;				// actual sampler time (in samples)
	double realtime;			// real sampler time
	
	// set sampler phase
	// use with FL_PHASE
	double desired_phase[2];	// phase/offset (in samples)
	int desired_volume[2];		// x.10 fixed point

	// position controls phase and time
	// use with FL_3DPOSITION
	cml::vector3d position;		// 3d position

	// ????
	float volume_scaler[2];		// position volume scaler

	// other FLAGS that can be used:
	// FL_STOP: stop the sound
	// FL_GHOST: create a ghost for the sound
	//   the ghost is created and the channel is reset before anything else is done
	// FL_DELETE: ???
	// FL_FORCEVOLUME: bypass volume ramping
	// FL_FORCEPHASE: bypass phase ramping
	// FL_FORCETIME: ???

	//  ************************
	// ***NOT USED BY CLIENT***
	//************************
	double phase[2];			// phase/offset (in samples)
	int volume[2];		
	int mode; // CHMODE_*
	bool dynamic_position;
} ChannelState;

//-----------------------------------------------------------------------------
class Channel { // mixer channel
//-----------------------------------------------------------------------------

private:
	
	ChannelState mixer_state;	// <- mixer side copy
	ChannelState client_state;	// <- client side copy

	CRITICAL_SECTION lock;

	// auto-delete after reaching end of source
	bool auto_delete;

	bool delete_flag;

public:

	Channel( bool p_auto_delete = true );
	~Channel();

	//----------------------------------------------------------------
	// apply entire channel state
	//
	// this is to setup any or all parameters
	// of the channel atomically atomic
	//
	void ApplyChannelState( const ChannelState &state );

	//----------------------------------------------------------------
	// set sampling source
	// also disables channel
	//
	void SetSource( const Sample *sample );

	//----------------------------------------------------------------
	// direct=true: set sampling rate directly (hz)
	// direct=false: set sampling rate relative to source rate (factor)
	// source being the sample being used
	//
	void SetSamplingRate( double sampling_rate, bool direct );

	//----------------------------------------------------------------
	// set time offset, in samples
	//
	// this is a delay after Start() is called before the sound will
	// actually be started
	// (for delaying sound effects which are played far away)
	//
	void SetTime( double time );

	//----------------------------------------------------------------
	// set speaker phases [offsets] in samples
	// 
	void SetPhase( double left, double right );
	void SetPhase( double offset );

	//----------------------------------------------------------------
	// set position of sound in world space
	// this function overrides the speaker phases and time offset
	// to simulate the 3d positioning
	//
	void Set3DPosition( cml::vector3d position );

	//----------------------------------------------------------------
	// set speaker output volume factors
	//
	// range = 0.0 - 2.0
	//
	void SetVolume( float left, float right );
	void SetVolume( float volume );

	//----------------------------------------------------------------
	// start sound
	//
	void Start();

	//----------------------------------------------------------------
	// stop sound (click free)
	//
	// will cause clicks if another sample is played immediately after
	// use Ghost() for that case
	//
	void Stop();

	//----------------------------------------------------------------
	// stop and delete channel (memory freed)
	//
	// the delete is a buffered action that happens after the channel
	// fades out (this function is click free)
	//
	void StopAndDelete();

	//----------------------------------------------------------------
	// stop channel with a ghost
	// ghosts are copies of a channel that fade out to avoid clicks
	//
	// to stop an existing sound and play a new sound, create a ghost
	// first to fade the existing sound out, and after the ghost is
	// created the original channel is reset and can play another
	// sound
	//
	void Ghost();

	//----------------------------------------------------------------
	// set the auto delete feature
	//
	// if auto delete is set and a oneshot sound ends, the channel
	// will be deleted by the system
	//
	// otherwise the channel may be reused or must be manually
	// deleted by the user
	//
	void SetAutoDelete(bool);

	//----------------------------------------------------------------
	// add to mixer
	// must be used after creating channel (ideally after setting up a sound)
	void Add();

	//----------------------------------------------------------------
	// system side only
	//
	void Lock();
	void Unlock();
	ChannelState *GetMixerState();
	ChannelState *GetClientState();
	void CopyClientState( ChannelState &target );
	void ResetClientFlags();
	void Update3D(bool time_offset );
	//void SetSoundController( Sound *controller );

	//----------------------------------------------------------------
	void SetDeleteFlag();
	bool ReadDeleteFlag() const;
	//----------------------------------------------------------------
	void AutoDeleteFunction() {
		if( auto_delete ) SetDeleteFlag();
	}

	//----------------------------------------------------------------
	enum {
		// see ChannelState
		FL_START  = 0x1,
		FL_STOP   = 0x2,
		FL_RATE   = 0x4,
		FL_PHASE  = 0x8,
		FL_TIME   = 0x10,
		FL_VOLUME = 0x20,
		FL_GHOST  = 0x40,
		FL_SOURCE = 0x80,
		FL_DELETE = 0x100,
		FL_FORCEVOLUME = 0x200,
		FL_3DPOSITION = 0x400,
		FL_FORCEPHASE = 0x800,
		FL_FORCETIME = 0x1000
	};

	//----------------------------------------------------------------
	enum {
		MODE_DISABLED, // no mixing, inactive
		MODE_ACTIVE,   // active and mixing normally
		MODE_STOPPING, // fading to 0 followed by disabling
		MODE_DELETING, // fading to 0 followed by disabling and deleting
	};

	// linked list pointers
	Channel *prev;
	Channel *next;
};


//-------------------------------------------------------------------------------------------------
}

#endif
