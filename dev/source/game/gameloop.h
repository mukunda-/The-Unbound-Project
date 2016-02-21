//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2016, Mukunda Johnson, All rights reserved. =========//

#pragma once

//-----------------------------------------------------------------------------
namespace Game {

/** ---------------------------------------------------------------------------
 * A game loop handles precise timing and fires periodic update intervals.
 *
 * It also monitors the video status and issues "draw" ticks to refrehsh
 * the video data, and handles frame-skip to allow smooth logic processing.
 *
 * Refresh rate for logic and video should be controllable by the user.
 */
class GameLoop {
	
public:
	using Handler  = std::function<bool()>;
	using Timer    = boost::asio::high_resolution_timer;
	using TimerPtr = std::shared_ptr<Timer>;
	using Clock    = std::chrono::high_resolution_clock;

	/** -----------------------------------------------------------------------
	 * Create a game loop.
	 * 
	 * @param refresh_rate Rate at which ticks are processed.
	 */
	GameLoop( double refresh_rate );
	virtual ~GameLoop();

	/** -----------------------------------------------------------------------
	 * Start the game loop. The handlers will be called periodically.
	 */
	void Start();

	//-------------------------------------------------------------------------
private:
	
	bool   m_active;			// if the timer is active
	double m_rate;
	double m_period;
	int    m_period_us;
	bool   m_main;
	Timer  m_timer;

	Clock::time_point m_next_tick;

	// to stop the game loop, return false in the tick handler
	void Stop();

	void OnTick( const boost::system::error_code &err );
};

//-------------------------------------------------------------------------------
}
