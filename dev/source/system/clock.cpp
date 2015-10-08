//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2015, Mukunda Johnson, All rights reserved. =========//

#include "stdafx.h"
#include "clock.h"
#include "util/minmax.h"

// time in which the clock should be re-synced
#define SYNC_THRESHOLD 0.25

namespace System {

using namespace std;
using namespace std::chrono; 

//-----------------------------------------------------------------------------
Clock::Clock() {}
Clock::~Clock() {}

//-----------------------------------------------------------------------------
void Clock::Reset() {
	m_start        = Source::now();
	m_steady_start = SteadySource::now();
}

//-----------------------------------------------------------------------------
double Clock::GetTime() {
	double time = (double)(duration_cast<microseconds>( 
			Source::now() - m_start ).count());
	
	double stime = (double)(duration_cast<microseconds>( 
			SteadySource::now() - m_steady_start ).count());
	
	if( fabs( time - stime ) > SYNC_THRESHOLD ) {
		// clock went out of sync with steady source, adjust it.

		m_start = Source::now() - (SteadySource::now() - m_steady_start);

		time = (double)(duration_cast<microseconds>( 
							Source::now() - m_start ).count());
	}

	time = time / 1000000.0;

	// guarantee that we don't return a time sooner than the last.
	time = Util::Max( time, m_last_time );

	m_last_time = time;

	return time;
}

}
