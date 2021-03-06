//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright � 2015, Mukunda Johnson, All rights reserved. =========//

#pragma once

namespace System {

/** ---------------------------------------------------------------------------
 * A Pulse keeps track of time for doing tick processing.
 */
class Pulse {
 
public:
	
	using Handler  = std::function<void()>;
	using Timer    = boost::asio::steady_timer;
	using TimerPtr = std::shared_ptr<Timer>;
	using Clock    = std::chrono::steady_clock;

	/** -----------------------------------------------------------------------
	 * Create a pulse.
	 *
	 * @param frequency Frequency at which the event should be generated.
	 * @param main      Use the main thread or main strand for executing
	 *                  handlers.
	 */
	Pulse( float frequency, bool main_thread = true );
	virtual ~Pulse();
	
	/** -----------------------------------------------------------------------
	 * Reset the time for the next tick.
	 */
	void Reset();

	/** -----------------------------------------------------------------------
	 * Wait until the next tick.
	 *
	 * @param callback Function to call after waiting.
	 */
	void Wait( Handler callback );

	//-------------------------------------------------------------------------
private:

	double m_freq;
	double m_period;
	int    m_period_us;
	bool   m_main;
	Timer  m_timer;

	Clock::time_point m_next_tick;

	void OnTick();
};

}