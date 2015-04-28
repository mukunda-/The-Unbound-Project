//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2015, Mukunda Johnson, All rights reserved. =========//

#include "stdafx.h"

#include "pulse.h"
#include "system.h"

//-----------------------------------------------------------------------------
namespace System {

//-----------------------------------------------------------------------------
Pulse::Pulse( float frequency, Func handler ) : 
		m_handler( handler ), m_timer( GetService()() ) {

	m_freq = frequency;
	m_period = 1.0 / m_freq;
	m_period_us = (int)(m_period * 1000000.0);

	Reset();
}

//-----------------------------------------------------------------------------
Pulse::~Pulse() {

}

//-----------------------------------------------------------------------------
void Pulse::Reset() {
	using namespace std::chrono; 
	m_next_tick = Clock::now() + microseconds( m_period_us );
}

void Pulse::Wait( Handler handler ) {

	auto timer = std::make_shared<Timer>( GetService()() );

	timer->expires_at( m_next_tick );
	// todo...
}

//-----------------------------------------------------------------------------
void Pulse::Start() {
	using namespace std::chrono; 

	m_next_tick = steady_clock::now() + microseconds( m_period_us );

	m_timer.expires_at( m_next_tick );
	m_timer.async_wait( std::bind( 
	//m_timer.expires_from_now( 
}

//-----------------------------------------------------------------------------
void Pulse::Stop() {

}

//-----------------------------------------------------------------------------
}