//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//
//-------------------------------------------------------------------------------------------------
#pragma once

//-------------------------------------------------------------------------------------------------

// transfer queue for transfering data between threads safely
//

//-------------------------------------------------------------------------------------------------
#ifndef AUDIO_TRANSFERQUEUE_H
#define AUDIO_TRANSFERQUEUE_H

//-------------------------------------------------------------------------------------------------
namespace Audio {

//-------------------------------------------------------------------------------------------------
template<class object> class TransferQueue {
	
private:
	object *first;
	object *last;
	CRITICAL_SECTION cs;
	int enabled;

public:
	TransferQueue() {
		enabled=1;
		InitializeCriticalSectionAndSpinCount(&cs, 0x00000400);
	}

	~TransferQueue() {
		enabled=0;
		DeleteCriticalSection(&cs);
	}

	// client side
	void Add( object *obj ) {
		if( !enabled ) return;

		obj->next = 0;

		EnterCriticalSection( &cs );

		if( !first ) {
			first=last=obj;
			obj->prev=0;
		} else {
			last->next = obj;
			obj->prev = last;
			last = last->next;
		}

		LeaveCriticalSection( &cs );
	}

	// system side
	void ReadAndReset( object *&r_first, object *&r_last ) {
		if( !enabled ) {
			r_first=r_last=0;
			return;
		}

		EnterCriticalSection( &cs );
		r_first = first;
		r_last = last;
		first=last=0;


		LeaveCriticalSection( &cs );

	}

};

//-------------------------------------------------------------------------------------------------
}

#endif
