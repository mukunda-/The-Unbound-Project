//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//

#pragma once

//-----------------------------------------------------------------------------
#ifndef AUDIO_SAMPLE_H
#define AUDIO_SAMPLE_H

#include "misc/types.h"

//-----------------------------------------------------------------------------
namespace Audio {

//-----------------------------------------------------------------------------
class Sample {

	float volume_scaler;
	float sampling_rate;
	s16 *data;

	int length;
	bool has_loop;
	int loop_start;
	int loop_length;

	bool deleting;

public:
	Sample();
	~Sample();

	void Erase();
	void CreateEmpty( int p_length, bool initialize = true );
	bool CreateFromWAV( const char *filename );
	
	void SetVolumeScaler( float vs );
	float GetVolumeScaler() const;
	void SetNormalSamplingRate( float sr );
	float GetNormalSamplingRate() const;

	void SetLoopRange( int start, int length );
	void EnableLoop( bool enabled );
	bool HasLoop() const;
	bool Oneshot() const;
	void GetLoopRange( int &r_start, int &r_length ) const;
	int LoopStart() const;
	int LoopLength() const;
	int Length() const; // returns loop end if loop enabled

	int Read( double position ) const;

	s16 *GetDataPointer();

	//void Add();
	void Delete();

	Sample *prev;
	Sample *next;
};

//-----------------------------------------------------------------------------
}

#endif
