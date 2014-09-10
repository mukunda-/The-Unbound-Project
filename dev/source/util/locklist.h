//===========================  The Unbound Project  =========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

#pragma once

//-----------------------------------------------------------------------------
#include "mem/memorylib.h"

//-----------------------------------------------------------------------------
namespace Util {
	
//-----------------------------------------------------------------------------
class LockList {
private:

	//-----------------------------------------------------------------------------------
	typedef struct t_Lock : public Memory::FastAllocation {
		boost::uint64_t id;
		
		struct t_Lock *prev;
		struct t_Lock *next;

	} Lock;

	Lock *first;
	Lock *last;
	
	boost::condition_variable cvar;
	boost::mutex cvar_mutex;
	
	Lock * FindLock( boost::uint64_t id ) const;
	void CreateLock( boost::uint64_t id );
	void RemoveLock( Lock *lock );
	void RemoveLock( boost::uint64_t id );

public:

	//----------------------------------------------------------------------------
	LockList();
	~LockList();
	
	/// --------------------------------------------------------------------------
	/// Acquire a lock on a table entry.
	/// This function will block until the table entry is available (if not already)
	///
	/// warning: deadlock if nested use within the same thread
	/// (will also deadlock if another thread never releases a lock)
	///
	void Acquire( boost::uint64_t id );

	/// --------------------------------------------------------------------------
	/// Release a table entry.
	///
	/// This must not be called if a lock wasn't acquired.
	///
	void Release( boost::uint64_t id );
};

}
