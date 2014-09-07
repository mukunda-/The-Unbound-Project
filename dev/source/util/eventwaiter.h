//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//

#pragma once

#include <boost/thread.hpp>

//-------------------------------------------------------------------------------------------------
namespace Util {

//-------------------------------------------------------------------------------------------------
class EventWaiter {
	
	int flags;

	boost::mutex mut;
	boost::condition_variable cvar;

public:

	EventWaiter();

	void Flag(int value);
	int Wait();
};


}
