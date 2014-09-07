//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//

#include "stdafx.h"

namespace Util {

EventWaiter::EventWaiter() {
	flags = 0;
}

void EventWaiter::Flag( int value ) {
	boost::lock_guard<boost::mutex> lock(mut);
	flags |= value;
	cvar.notify_one();
}

int EventWaiter::Wait() {
	boost::unique_lock<boost::mutex> lock(mut);
	while( !flags ) {
		cvar.wait(lock);
	}
	int result = flags;
	flags = 0;
	return result;
}

}
