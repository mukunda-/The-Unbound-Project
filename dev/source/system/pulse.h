//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2015, Mukunda Johnson, All rights reserved. =========//

#pragma once

namespace System {

/** ---------------------------------------------------------------------------
 * A Pulse posts a task on an interval.
 */
class Pulse {
 
public:

	using Func  = std::function<void()>;
	using Timer = boost::asio::high_resolution_timer;

	/** -----------------------------------------------------------------------
	 * Create a pulse.
	 *
	 * @param frequency Frequency at which the event should be generated.
	 * @param handler   Handler to call.
	 */
	Pulse( float frequency, Func handler );
	virtual ~Pulse();
	
	/** -----------------------------------------------------------------------
	 * Start the loop.
	 */
	void Start();

	/** -----------------------------------------------------------------------
	 * Stop the loop.
	 */
	void Stop();

	//-------------------------------------------------------------------------
private:

	double m_freq;
	double m_period;
	Func   m_handler;
	Timer  m_timer;

	std::chrono::time_point<std::chrono::high_resolution_clock> m_next_tick;

	void OnTick();
};

}