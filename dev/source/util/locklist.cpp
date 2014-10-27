//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

#include "stdafx.h"
#include "util/locklist.h"

namespace Util {

//-------------------------------------------------------------------------------------------------
LockList::Lock * LockList::FindLock( boost::uint64_t id ) const {
	for( Lock *lock = first; lock; lock = lock->next ) {
		if( lock->id == id ) return lock;
	}
	return 0;
}

//-------------------------------------------------------------------------------------------------
void LockList::CreateLock( boost::uint64_t id ) {
	Lock *lock = new Lock;
	lock->id = id;
	lock->next = 0;
	
	if( !last ) {
		first = last = lock;
		lock->prev = 0;
	} else {
		last->next = lock;
		lock->prev = last;
		last = lock;
	}
}

//-------------------------------------------------------------------------------------------------
void LockList::RemoveLock( Lock *lock ) {
	if( lock->next ) {
		lock->next->prev = lock->prev;
	} else {
		last = lock->prev;
	}

	if( lock->prev )
		lock->prev->next = lock->next;
	else
		first = lock->next;

	delete lock;
}

//-------------------------------------------------------------------------------------------------
void LockList::RemoveLock( boost::uint64_t id ) {
	Lock *lock = FindLock( id );
	RemoveLock( lock );
}

//-------------------------------------------------------------------------------------------------
LockList::LockList() {
	first = last = 0;
}

//-------------------------------------------------------------------------------------------------
LockList::~LockList() {
	while( first ) {
		RemoveLock(first);
	}
}

//-------------------------------------------------------------------------------------------------
void LockList::Acquire( boost::uint64_t id ) {
	boost::unique_lock<boost::mutex> lock(cvar_mutex);

	while( FindLock( id ) ) {
		cvar.wait(lock);
	}

	CreateLock( id );
}

//-------------------------------------------------------------------------------------------------
void LockList::Release( boost::uint64_t id ) {
	{
		boost::lock_guard<boost::mutex> lock(cvar_mutex);
		RemoveLock( id );
	}

	cvar.notify_all();
}

//-------------------------------------------------------------------------------------------------
}
