//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2015, Mukunda Johnson, All rights reserved. =========//

#include "stdafx.h"

#include "pulse.h"
#include "system.h"

//-----------------------------------------------------------------------------
namespace System {

//-----------------------------------------------------------------------------
Pulse::Pulse( float frequency, bool main_thread ) : 
		m_timer( GetService()() ), m_main(main_thread) {

	m_freq = frequency;
	m_period = 1.0 / m_freq;
	m_period_us = (int)(m_period * 1000000.0);

	Reset();
}

//-----------------------------------------------------------------------------
Pulse::~Pulse() {}

//-----------------------------------------------------------------------------
void Pulse::Reset() {
	using namespace std::chrono; 
	m_next_tick = Clock::now() + microseconds( m_period_us );
}

//-----------------------------------------------------------------------------
void Pulse::Wait( Handler handler ) {
	using namespace std::chrono; 

	auto timer = std::make_shared<Timer>( GetService()() );

	timer->expires_at( m_next_tick );
	m_next_tick += microseconds( m_period_us );
	
	bool main_thread = m_main;

	timer->async_wait( 
		[timer,handler,main_thread]( const boost::system::error_code &err ) {

		if( err ) return; // (TODO)

		if( main_thread ) {
			System::Post( handler, true );
		} else {
			handler();
		}
	});
}
  
//-----------------------------------------------------------------------------
}