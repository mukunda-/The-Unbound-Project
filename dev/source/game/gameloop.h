//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2015, Mukunda Johnson, All rights reserved. ==========//

#pragma once

//-------------------------------------------------------------------------------
namespace Game {

/** -----------------------------------------------------------------------------
 * A game loop handles precise timing and fires periodic update intervals.
 *
 * It also monitors the video status and issues "draw" ticks to refrehsh
 * the video data, and handles frame-skip to allow smooth logic processing.
 *
 * Refresh rate for logic and video should be controllable by the user.
 */
class GameLoop {

	double m_rate;
	
public:
	using Handler  = std::function<bool()>;
	using Timer    = boost::asio::high_resolution_timer;
	using TimerPtr = std::shared_ptr<Timer>;
	using Clock    = std::chrono::high_resolution_clock;

	/** -------------------------------------------------------------------------
	 * Create a game loop.
	 * 
	 * @param refresh_rate Rate at which ticks are processed.
	 */
	GameLoop( double refresh_rate );
	virtual ~GameLoop();

	void Reset( float refresh_rate );
};

//-------------------------------------------------------------------------------
}
