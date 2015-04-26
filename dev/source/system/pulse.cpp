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
	
}

//-----------------------------------------------------------------------------
Pulse::~Pulse() {

}

//-----------------------------------------------------------------------------
void Pulse::Start() {
	m_next_tick = std::chrono::high_resolution_clock::now();

	m_timer.expires_from_now( 
}

//-----------------------------------------------------------------------------
void Pulse::Stop() {

}

//-----------------------------------------------------------------------------
}