//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2015, Mukunda Johnson, All rights reserved. =========//

#pragma once

namespace System {

/** ---------------------------------------------------------------------------
 * A Clock keeps track of time.
 */
class Clock {
//-----------------------------------------------------------------------------
public:
	using Source       = std::chrono::high_resolution_clock;

	// A steady source is used to verify that the high resolution source
	// wasn't tampered with.
	using SteadySource = std::chrono::steady_clock;

	//-------------------------------------------------------------------------
	Clock();
	virtual ~Clock();

	/** -----------------------------------------------------------------------
	 * Returns the amount of seconds elapsed since the clock was created or
	 * the last time `Reset` was called.
	 *
	 * `GetTime` will never return a value less than a previous call 
	 * to `GetTime` unless `Reset` is called.
	 */
	double GetTime();

	/** -----------------------------------------------------------------------
	 * Reset the clock. GetTime() returns the time since the last Reset().
	 */
	void Reset();

//-----------------------------------------------------------------------------
private:
	Source::time_point       m_start;
	SteadySource::time_point m_steady_start;

	double m_last_time;
};

}
