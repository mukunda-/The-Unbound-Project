//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2015, Mukunda Johnson, All rights reserved. ==========//

#include "stdafx.h"
#include "gameloop.h"
#include "system/system.h"

//-------------------------------------------------------------------------------
namespace Game {

//-------------------------------------------------------------------------------
GameLoop::GameLoop( double refresh_rate ) : 
	m_rate( refresh_rate ), m_timer( System::GetService()() ) {

	m_period = 1.0 / m_rate;
	m_period_us = (int)(m_period * 1000000.0);
}

//-----------------------------------------------------------------------------
void GameLoop::Start() {
	using namespace std::chrono;

	m_next_tick = Clock::now() + microseconds( m_period_us );

	m_timer.expires_at( m_next_tick );
	m_next_tick += microseconds( m_period_us );

	bool main_thread = m_main;

	m_timer.async_wait( [&]( const boost::system::error_code &err ) {

	});
}

//-----------------------------------------------------------------------------
void GameLoop::Stop() {
	
}

}
