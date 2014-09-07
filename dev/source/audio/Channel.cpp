//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//

//-------------------------------------------------------------------------------------------------
#include "stdafx.h"

//-------------------------------------------------------------------------------------------------
namespace Audio {

extern AudioGlobalSettings global_settings_copy;
extern TransferQueue<Channel> new_channels;
extern cml::vector3d listener[2];
extern double mixer_output_rate;

//-------------------------------------------------------------------------------------------------
Channel::Channel( bool p_auto_delete ) {
	memset( &mixer_state, 0, sizeof( ChannelState ) );
	memset( &client_state, 0, sizeof( ChannelState ) );

	mixer_state.volume_scaler[0] = mixer_state.volume_scaler[1] = 1.0f;
	client_state.volume_scaler[0] = client_state.volume_scaler[1] = 1.0f;

	InitializeCriticalSectionAndSpinCount(&lock, 0x00000400);

	auto_delete = p_auto_delete;
	delete_flag=false;
}

//-------------------------------------------------------------------------------------------------
Channel::~Channel() {
	// nothing to delete
}

//-------------------------------------------------------------------------------------------------
void Channel::Add() {
	new_channels.Add(this);
}

//-------------------------------------------------------------------------------------------------
void Channel::SetSource( const Sample *sample ) {
	Lock();
	client_state.flags |= FL_SOURCE;
	client_state.source = sample;
	Unlock();
}

//-------------------------------------------------------------------------------------------------
void Channel::SetSamplingRate( double sampling_rate, bool direct ) {
	Lock();
	client_state.flags |= FL_RATE;
	client_state.sampling_rate = sampling_rate;
	client_state.sampling_rate_direct = direct;
	Unlock();
}

//-------------------------------------------------------------------------------------------------
void Channel::SetTime( double time ) {
	Lock();
	client_state.flags |= FL_TIME;
	client_state.time = time;
	Unlock();
}

//-------------------------------------------------------------------------------------------------
void Channel::SetPhase( double left, double right ) {
	Lock();
	client_state.phase[0] = left;
	client_state.phase[1] = right;
	client_state.flags |= FL_PHASE;
	Unlock();
}

//-------------------------------------------------------------------------------------------------
void Channel::SetPhase( double offset ) {
	SetPhase( offset, offset );
}

//-------------------------------------------------------------------------------------------------
void Channel::Set3DPosition( cml::vector3d position ) {
	Lock();
	client_state.position = position;
	client_state.flags |= FL_3DPOSITION;
	Unlock();
}

//-------------------------------------------------------------------------------------------------
void Channel::SetVolume( float left, float right ) {
	Lock();
	client_state.desired_volume[0] = (int)(left * 1024.0);
	client_state.desired_volume[1] = (int)(left * 1024.0);
	client_state.flags |= FL_VOLUME;
	Unlock();
}

//-------------------------------------------------------------------------------------------------
void Channel::SetVolume( float volume ) {
	SetVolume( volume, volume );
}

//-------------------------------------------------------------------------------------------------
void Channel::Start() {
	Lock();
	client_state.flags |= FL_START;
	Unlock();
}

//-------------------------------------------------------------------------------------------------
void Channel::Stop() {
	Lock();
	client_state.flags |= FL_STOP;
	Unlock();
}

//-------------------------------------------------------------------------------------------------
void Channel::StopAndDelete() {
	Lock();
	client_state.flags |= FL_DELETE;
	Unlock();
}

//-------------------------------------------------------------------------------------------------
void Channel::Ghost() {
	Lock();
	client_state.flags |= FL_GHOST;
	Unlock();
}

//-------------------------------------------------------------------------------------------------
void Channel::SetAutoDelete( bool p ) {
	auto_delete = p;
}

//-------------------------------------------------------------------------------------------------
void Channel::Lock() {
	EnterCriticalSection( &lock );
}

//-------------------------------------------------------------------------------------------------
void Channel::Unlock() {
	LeaveCriticalSection( &lock );
}

//-------------------------------------------------------------------------------------------------
ChannelState * Channel::GetMixerState() {
	return &mixer_state;
}

//-------------------------------------------------------------------------------------------------
ChannelState * Channel::GetClientState() {
	return &client_state;
}

//-------------------------------------------------------------------------------------------------
void Channel::CopyClientState( ChannelState &target ) {
	// should be in locked state
	memcpy( &target, &client_state, sizeof( ChannelState ) );
}

//-------------------------------------------------------------------------------------------------
void Channel::ResetClientFlags() {
	// should be in locked state
	client_state.flags = 0;
}

//-------------------------------------------------------------------------------------------------
void Channel::Update3D( bool time_offset ) {
	double C = 340.29; // speed of sound

	cml::vector3d vec_left, vec_right;
	vec_left = mixer_state.position - listener[0];
	vec_right = mixer_state.position - listener[1];

	double dist[2];
	dist[0] = cml::length( vec_left );
	dist[1] = cml::length( vec_right );

	

	double pdist[2];

	pdist[0] = -(dist[0] / C * mixer_output_rate);
	pdist[1] = -(dist[1] / C * mixer_output_rate);

	if( time_offset ) {
		double sr;
		if( mixer_state.sampling_rate_direct || !mixer_state.source )
			sr = mixer_state.sampling_rate;
		else {
			sr = mixer_state.source->GetNormalSamplingRate() * mixer_state.sampling_rate;
		}
		sr /= mixer_output_rate;
		mixer_state.time = (pdist[0] * sr) / 4.0; 
	}

	pdist[0] -= pdist[1];
	pdist[1] = -pdist[0] / 2.0;
	pdist[0] -= pdist[0] / 2.0;
	
	mixer_state.desired_phase[0] = pdist[0];
	mixer_state.desired_phase[1] = pdist[1];
	
	vec_left.normalize();
	vec_right.normalize();


	float cos_a,cos_b,cos_c;//, cos_d ; // left, right, front, up
	cos_a = (float)cml::dot( -global_settings_copy.listener_right, vec_left );
	cos_b = (float)cml::dot( global_settings_copy.listener_right, vec_right );

	cml::vector3d vec_center =( vec_left + vec_right) / 2.0; 
	cos_c = (float)cml::dot( global_settings_copy.listener_forward, vec_center ); 
	//cos_d = cml::dot( settings.listener_up, vec_center );

	cos_a = 1.0f - ((cos_a + 1.0f) / 2.0f);
	cos_b = 1.0f - ((cos_b + 1.0f) / 2.0f);
	cos_c = 1.0f - ((cos_c + 1.0f) / 2.0f);
	//cos_d = 1.0f - ((cos_d + 1.0f) / 2.0f);
	
	float vleft, vright;
	vleft = vright = 1.0;

	vleft *= 1.0f - (cos_a * 0.5f);
	vright *= 1.0f - (cos_b * 0.5f);
	
	vleft *= 1.0f - cos_c * 0.25f;
	vright *= 1.0f - cos_c * 0.25f;

	//vleft *= 1.0f - cos_d * 0.1f;
	//vright *= 1.0f - cos_d * 0.1f;

	float ld,rd;
	ld = (float)(54.0 / (dist[0]));
	if( ld > 1.0f ) ld = 1.0f;
	rd = (float)(54.0 / (dist[1]));
	if( rd > 1.0f ) rd = 1.0f;
	
	mixer_state.volume_scaler[0] = vleft * ld;
	mixer_state.volume_scaler[1] = vright * rd;
	

}

//-------------------------------------------------------------------------------------------------
void Channel::SetDeleteFlag() {
	delete_flag = true;
}

//-------------------------------------------------------------------------------------------------
bool Channel::ReadDeleteFlag() const {
	return delete_flag;
}

}
