//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//

#pragma once

#include <boost/asio.hpp>

namespace Util {

//------------------------------------------------------------------------------
class ThreadState {

private:
	//------------------------------------------------------------------------------
	boost::mutex mut;
	boost::condition_variable cvar;
	int state;
public:

	//------------------------------------------------------------------------------
	ThreadState() {
		state = 0;
	}

	//------------------------------------------------------------------------------
	void Set( int new_state ) {
		{
			boost::lock_guard<boost::mutex> lock(mut);
			state = new_state;
		}
		cvar.notify_all();
	}

	//------------------------------------------------------------------------------
	int Get() {
		boost::lock_guard<boost::mutex> lock(mut);
		return state;
	}

	//------------------------------------------------------------------------------
	void Wait( int until_state ) {
		boost::unique_lock<boost::mutex> lock(mut);
		while( state != until_state ) {
			cvar.wait(lock);
		}
	}

	//------------------------------------------------------------------------------
	void SetFlags( int flags ) {
		boost::lock_guard<boost::mutex> lock(mut);
		state |= flags;
		cvar.notify_all();
	}

	//------------------------------------------------------------------------------
	void ClearFlags( int flags ) {
		boost::lock_guard<boost::mutex> lock(mut);
		state &= ~flags;
	}

	//------------------------------------------------------------------------------
	void WaitFlags( int flags_set, int flags_unset=0 ) {
		boost::unique_lock<boost::mutex> lock(mut);
		while( (state & flags_set) != flags_set || ((state & flags_unset) != 0) ) {
			cvar.wait(lock);
		}
	}
};

}

