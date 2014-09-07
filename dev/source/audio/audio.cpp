//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//
//-------------------------------------------------------------------------------------------------

#include "stdafx.h"

//-------------------------------------------------------------------------------------------------
namespace Audio {

//-------------------------------------------------------------------------------------------------
double mixer_output_rate = 44100.0;

//-------------------------------------------------------------------------------------------------
// CLIENT SIDE
TransferQueue<Channel> new_channels;
TransferQueue<Sample> sample_garbage;

bool enabled = false;
bool sdl_init = false;

//-------------------------------------------------------------------------------------------------
// SYSTEM SIDE
AudioList<Channel> channels;
AudioList<Sample> samples_to_delete;

AudioGlobalSettings global_settings;
AudioGlobalSettings global_settings_copy;
CRITICAL_SECTION global_settings_lock;

cml::vector3d listener[2];

//-------------------------------------------------------------------------------------------------
enum {
	BUFFER_LENGTH = 1024, // length of sound buffers

};

//-------------------------------------------------------------------------------------------------
_CRT_ALIGN(16) int buffer_left[BUFFER_LENGTH];
_CRT_ALIGN(16) int buffer_right[BUFFER_LENGTH];

//-----------------------------------------------------------------------------
float GetAudioTime() {
//-----------------------------------------------------------------------------
	return (float)clock() / (float)CLOCKS_PER_SEC;
}

//-----------------------------------------------------------------------------
void LockGlobalSettings() {
//-----------------------------------------------------------------------------
	EnterCriticalSection( &global_settings_lock );
}

//-----------------------------------------------------------------------------
void UnlockGlobalSettings() {
//-----------------------------------------------------------------------------
	LeaveCriticalSection( &global_settings_lock );
}

//-----------------------------------------------------------------------------
void ClearBuffers() {
//-----------------------------------------------------------------------------
	__m128i zero = _mm_setzero_si128();
	__m128i *left = (__m128i*)buffer_left;
	__m128i *right = (__m128i*)buffer_right;
	for( int i = BUFFER_LENGTH/4; i; i-- ) {
		_mm_store_si128( left, zero );
		_mm_store_si128( right, zero );
		left++;
		right++;
	}
}

//-----------------------------------------------------------------------------
void TransferClientStructs() {
//-----------------------------------------------------------------------------
	{
		Channel *first, *last;
		new_channels.ReadAndReset( first, last );
		channels.AppendList( first, last );
	}
	
	//{
	//	Sound *first, *last;
	//	new_sounds.ReadAndReset( first, last );
	//	sounds.AppendList( first, last );
	//}

	{
		Sample *first, *last;
		sample_garbage.ReadAndReset( first, last );
		samples_to_delete.AppendList( first, last );
	}
}

//-----------------------------------------------------------------------------
void CutChannelsUsingSample( Sample *sample ) {
//-----------------------------------------------------------------------------
	for( Channel *ch = channels.GetFirst(); ch; ch = ch->next ) {
		ch->Lock();

		// cancel mixer state
		ChannelState *state = ch->GetMixerState();
		if( state->source == sample ) {
			state->source = 0;
			state->mode = Channel::MODE_DISABLED;
			state->mode &= ~(Channel::FL_START+Channel::FL_GHOST);
		}

		// cancel client state
		state = ch->GetClientState();
		if( state->source == sample ) {
			state->source = 0;
			state->flags &= ~(Channel::FL_START+Channel::FL_GHOST);
		}

		ch->Unlock();
	}
}

//-----------------------------------------------------------------------------
void DeleteChannels() {
//-----------------------------------------------------------------------------
	for( Channel *ch = channels.GetFirst(); ch; ) {
		Channel *next = ch->next;
		bool delete_flag = ch->ReadDeleteFlag();
		if( delete_flag ) {
			channels.Remove( ch );
			Memory::Free( ch );
			//delete ch;
		}

		ch = next;
	}
}

//-----------------------------------------------------------------------------
void DeleteSamples() {
//-----------------------------------------------------------------------------
	// take out the trash

	for( Sample *sample = samples_to_delete.GetFirst(); sample; ) {
		Sample *next = sample->next;

		CutChannelsUsingSample( sample );
		samples_to_delete.Remove( sample );
		//delete sample;
		Memory::Free( sample );
		sample = next;
		
	}
}

//-----------------------------------------------------------------------------
void CopyChannelClientStates() {
//-----------------------------------------------------------------------------

	// todo: why is all this shit outside of the channe lclass


	for( Channel *ch = channels.GetFirst(); ch; ch = ch->next ) {

		// READ CHANNEL STATE
		ch->Lock();
		ChannelState cs;
		ch->CopyClientState( cs );
		ch->ResetClientFlags();
		ch->Unlock();

		// APPLY CHANNEL STATE
		ChannelState *ms = ch->GetMixerState();

		if( cs.flags & Channel::FL_GHOST ) {
			// todo: ghosts
			ms->mode = Channel::MODE_DISABLED;
			ms->source = 0;
		}

		if( cs.flags & Channel::FL_SOURCE ) {
			ms->mode = Channel::MODE_DISABLED;
			ms->source = cs.source;
			ms->time = 0;
			//cs.flags |= CF_START;
			//cs.flags &= ~CF_STOP;
			
		}

		if( cs.flags & Channel::FL_START ) {
			ms->mode = Channel::MODE_ACTIVE;
			cs.flags |= Channel::FL_FORCEVOLUME | Channel::FL_FORCEPHASE;
			cs.flags &= ~Channel::FL_STOP;

		}

		if( cs.flags & Channel::FL_STOP ) {
			ms->mode = Channel::MODE_STOPPING;
		}
		
		if( cs.flags & Channel::FL_DELETE ) {
			ms->mode = Channel::MODE_DELETING;
		}

		if( cs.flags & Channel::FL_RATE ) {
			ms->sampling_rate = cs.sampling_rate;
			ms->sampling_rate_direct = cs.sampling_rate_direct;
		}

		if( cs.flags & Channel::FL_PHASE ) {
			ms->desired_phase[0] = cs.phase[0];
			ms->desired_phase[1] = cs.phase[1];
			ms->dynamic_position = false;
		}

		if( cs.flags & Channel::FL_3DPOSITION || ms->dynamic_position ) {
			ms->dynamic_position = true;
			ms->position = cs.position;
			ch->Update3D( cs.flags & Channel::FL_START );
		}
		
		if( cs.flags & Channel::FL_FORCEPHASE ) {
			ms->phase[0] = ms->desired_phase[0];
			ms->phase[1] = ms->desired_phase[1];
		}
		
		if( cs.flags & Channel::FL_TIME ) {
			ms->time = cs.time;
		}

		if( cs.flags & Channel::FL_VOLUME ) {
			ms->desired_volume[0] = cs.desired_volume[0];
			ms->desired_volume[1] = cs.desired_volume[1];
			
		}

		if( cs.flags & Channel::FL_FORCEVOLUME ) {
			ms->volume[0] = ms->desired_volume[0];
			ms->volume[1] = ms->desired_volume[1];
		}
	}
}

//-----------------------------------------------------------------------------
void MixChannelStereoPhase( Channel *ch, int samples ) {
//-----------------------------------------------------------------------------
	ChannelState *cs = ch->GetMixerState();
	if( cs->mode == Channel::MODE_DISABLED || !cs->source ) {
		// channel is disabled, skip mixing
		return;
	}
	
	const Sample *source = cs->source;
	
	double rate[2];// = cs->sampling_rate_direct ? (cs->sampling_rate) : (cs->sampling_rate * source->GetNormalSamplingRate());
	rate[0] = cs->sampling_rate_direct ? (cs->sampling_rate) : (cs->sampling_rate * source->GetNormalSamplingRate());
	rate[0] /= (double)mixer_output_rate;
	
	double sampletime[2];
	double sampletime_end[2];
	sampletime[0] = cs->time + cs->phase[0] * rate[0];
	sampletime[1] = cs->time + cs->phase[1] * rate[0];

	//cs->phase[0] = cs->phase[0] * 0.95 + cs->desired_phase[0] * 0.05;
	//cs->phase[1] = cs->phase[1] * 0.95 + cs->desired_phase[1] * 0.05;

	cs->time += rate[0] * samples;
	rate[1] = rate[0];
	
	cs->phase[0] = cs->desired_phase[0];//cs->phase[0] * 0.8 + cs->desired_phase[0] * 0.2;
	cs->phase[1] = cs->desired_phase[1];//phase[1] * 0.8 + cs->desired_phase[1] * 0.2;
	
	sampletime_end[0] = cs->time + cs->phase[0] * rate[0];
	sampletime_end[1] = cs->time + cs->phase[1] * rate[0];
	
	rate[0] = (sampletime_end[0] - sampletime[0]) / (double)samples;
	rate[1] = (sampletime_end[1] - sampletime[1]) / (double)samples;
	


	//rate = rate / MixerOutputRate;
	double end = (double)source->Length();

	if( source->HasLoop() ) {
		if( sampletime[0] >= end ) {
			sampletime[0] = fmod(sampletime[0] - source->LoopStart(), source->LoopLength() ) + source->LoopStart();
		}
		if( sampletime[1] >= end ) {
			sampletime[1] = fmod(sampletime[1] - source->LoopStart(), source->LoopLength() ) + source->LoopStart();
		}
	} else {
		if( sampletime[0] >= end && sampletime[1] >= end ) {
			cs->mode = Channel::MODE_DISABLED;
			ch->AutoDeleteFunction();
			return;
		}
	}

	

	// volume ramping (TODO)
	float desired_volume[2];
	if( cs->mode == Channel::MODE_DELETING || cs->mode == Channel::MODE_STOPPING ) {
		desired_volume[0] = 0;
		desired_volume[1] = 0;
	} else {
		desired_volume[0] = cs->desired_volume[0] * cs->volume_scaler[0];
		desired_volume[1] = cs->desired_volume[1] * cs->volume_scaler[1];
	}

	cs->volume[0] = (int)desired_volume[0];
	cs->volume[1] = (int)desired_volume[1];
	//

	if( cs->mode == Channel::MODE_DELETING || cs->mode == Channel::MODE_STOPPING ) {
		if( cs->volume[0] == 0 && cs->volume[1] == 0 ) {
			if( cs->mode == Channel::MODE_DELETING ) {
				ch->SetDeleteFlag();
			}
			cs->mode = Channel::MODE_DISABLED;
			ch->AutoDeleteFunction();

		}
	}

	for( int i = 0; i < samples; i++ ) {
		int sample[2];

		sample[0] = source->Read( sampletime[0] );
		sample[1] = source->Read( sampletime[1] );
		sample[0] = sample[0] * cs->volume[0]; // mul by volume (.10)
		sample[1] = sample[1] * cs->volume[1]; // = (.26)
		sample[0] = sample[0] >> 8; // shift to (.18)
		sample[1] = sample[1] >> 8;
		buffer_left[i]  += sample[0];
		buffer_right[i] += sample[1];

		sampletime[0] += rate[0];
		if( sampletime[0] >= end ) {
			if( source->HasLoop() ) {
				sampletime[0] -= source->LoopLength();
			} else {
				cs->mode = Channel::MODE_DISABLED;
				ch->AutoDeleteFunction();
				break;
			}
		}

		sampletime[1] += rate[1];
		if( sampletime[1] >= end ) {
			if( source->HasLoop() ) {
				sampletime[1] -= source->LoopLength();
			} else {
				cs->mode = Channel::MODE_DISABLED;
				ch->AutoDeleteFunction();
				break;
			}
		}
	}
	 

	
}

//-----------------------------------------------------------------------------
void RunMixer( int samples ) {
//-----------------------------------------------------------------------------
	for( Channel *ch = channels.GetFirst(); ch; ch = ch->next ) {
		MixChannelStereoPhase( ch, samples );
	}
}

//-----------------------------------------------------------------------------
void OutputMixerData( int samples, s16 *output ) {
//-----------------------------------------------------------------------------

	LockGlobalSettings();
	double volume = global_settings.volume;
	UnlockGlobalSettings();

	for( int i = 0; i < samples; i++ ) {
		int sample[2];
		sample[0] = buffer_left[i];
		sample[1] = buffer_right[i];

		sample[0] *= (int)volume;
		sample[1] *= (int)volume;
		
		// shift
		sample[0] >>= 2;
		sample[1] >>= 2;

		// clamp
		if( sample[0] < -32767 ) sample[0] = -32767;
		if( sample[0] > 32767 ) sample[0] = 32767;
		if( sample[1] < -32767 ) sample[1] = -32767;
		if( sample[1] > 32767 ) sample[1] = 32767;

		// outpu
		*output++ = sample[0];
		*output++ = sample[1];
	}
}

//-----------------------------------------------------------------------------
void UpdateListener() {
//-----------------------------------------------------------------------------
	cml::vector3d center = global_settings_copy.listener_position;// listenerEnvelope.read( GetAudioTime() - 0.08 );
	
	listener[0] = center - (global_settings_copy.listener_right * global_settings_copy.listener_separation);
	listener[1] = center + (global_settings_copy.listener_right * global_settings_copy.listener_separation);

}

//-----------------------------------------------------------------------------
void UpdateAudio( int samples, s16 *output ) {
//-----------------------------------------------------------------------------
	//AudioGlobalSettings GlobalSettingsCopy;
	LockGlobalSettings();
	global_settings_copy = global_settings;
	//GlobalSettings.listener_changed = false; // clear flags
	UnlockGlobalSettings();

	UpdateListener();

	TransferClientStructs();
	CopyChannelClientStates();

	ClearBuffers();
	RunMixer( samples );
	OutputMixerData( samples, output );

	DeleteChannels();
	DeleteSamples();
}

//-----------------------------------------------------------------------------
void MixAudio_SDL( void *unused, u8 *stream, int len ) {
//-----------------------------------------------------------------------------
	s16 *data = (s16*)stream;

	UpdateAudio( len/4, (s16*)stream );

	
}

//-----------------------------------------------------------------------------
void Unload_SDL() {
//-----------------------------------------------------------------------------
	if( enabled ) {
		SDL_CloseAudio();
	}
	enabled = false;

	
}

//-----------------------------------------------------------------------------
void Initialize_SDL() {
//-----------------------------------------------------------------------------

	if( !sdl_init ) {
		SDL_Init( SDL_INIT_AUDIO );
		sdl_init=true;
	}
	Unload_SDL();

	SDL_AudioSpec fmt;
	memset( &fmt, 0, sizeof(fmt) );
	fmt.freq = 44100;
	fmt.format = AUDIO_S16;
	fmt.channels = 2;
	fmt.samples = 512;
	fmt.callback = MixAudio_SDL;
	fmt.userdata = NULL;
	
	if( SDL_OpenAudio( &fmt, NULL ) < 0 ) {
		// derp
		return;
	}

	SDL_PauseAudio(0);
}

//-----------------------------------------------------------------------------
void InitializeDevice() {
//-----------------------------------------------------------------------------
	Initialize_SDL();


}

//-----------------------------------------------------------------------------
void Initialize() {
//-----------------------------------------------------------------------------

	global_settings.volume = 1.0;

	InitializeCriticalSectionAndSpinCount(&global_settings_lock, 0x400);
//	listenerEnvelope.create( 10 );
	// todo, enumerate devices
	
	InitializeDevice();
}

//-----------------------------------------------------------------------------
void Unload() {
//-----------------------------------------------------------------------------
	Unload_SDL();
	 
	if( sdl_init ) {
		sdl_init=false;
		SDL_Quit();
	}
}
/*
//-----------------------------------------------------------------------------
Channel * CreateChannel( bool auto_delete ) {
//-----------------------------------------------------------------------------
	Channel *ch = new Channel();
	ch->SetAutoDelete( auto_delete );
	return ch;
}

//-----------------------------------------------------------------------------
void AddChannel( Channel *ch ) {
//-----------------------------------------------------------------------------
	new_channels.Add( ch );
}
*/
//-----------------------------------------------------------------------------
void StartSimple3D( const Sample *source, cml::vector3d position, float rate ) {
//-----------------------------------------------------------------------------
	Channel *ch = new(Memory::Alloc<Channel>()) Channel;//CreateChannel(true);
	ch->SetSource( source );
	ch->SetVolume( 1.0 );
	ch->SetSamplingRate(rate,false);
	ch->Set3DPosition( position );
	ch->Start();
	ch->Add();
	//AddChannel( ch );
}
 /*
//-----------------------------------------------------------------------------
void StartSimple3D( int sample_index, cml::vector3d position, float rate ) {
//-----------------------------------------------------------------------------
	StartSimple3D( SampleFromTable(sample_index), position, rate );
}*/

//-----------------------------------------------------------------------------
void SetListener2( cml::vector3d center, cml::vector3d forward, cml::vector3d right, double separation ) {
//-----------------------------------------------------------------------------
	LockGlobalSettings();

	//GlobalSettings.listener_velocity = center - GlobalSettings.listener_position;
	global_settings.listener_position = center;

	global_settings.listener_forward = forward;
	global_settings.listener_right = right;

	global_settings.listener_forward = cml::normalize(forward);
	global_settings.listener_right = cml::normalize(right);
	global_settings.listener_up = cml::cross( global_settings.listener_right, global_settings.listener_forward );
	global_settings.listener_up.normalize();
	global_settings.listener_separation = separation;

	UnlockGlobalSettings();
	/*
	Audio::EnvelopeKey dat;
	dat.data = center;
	dat.time = GetAudioTime();

	listenerEnvelope.add( dat );
	*/
}

}


