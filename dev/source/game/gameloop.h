//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2015, Mukunda Johnson, All rights reserved. ==========//

#pragma once

//-------------------------------------------------------------------------------
namespace Game {

/** -----------------------------------------------------------------------------
 * Yet another attempt at a game loop.
 */
class GameLoop {
	
public:
	using Handler  = std::function<void()>;
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
