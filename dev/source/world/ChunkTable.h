//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//

#if 0

#pragma once

// the Chunk Tree manages chunks in a ginormous world using a tree structure
//
// 

// tree format:
// trunk: 16x16x8
// level 1: 16x16x8
// level 2: 16x16x8
// level 3: 16x16x8	// (max 650x650x40 miles)

//------------------------------------------------------------------------------------------------------------------------
#include <boost/thread.hpp>
#include <boost/cstdint.hpp>
#include <boost/filesystem.hpp>
#include "world/Chunk.h"

//------------------------------------------------------------------------------------------------------------------------
namespace World {
	 
//------------------------------------------------------------------------------------------------------------------------
// ChunkTree 
class ChunkTree {

	//--------------------------------------------------------------------
	boost::mutex mut;
	boost::condition_variable cvar_lockchanges;
	
	//--------------------------------------------------------------------
	// branch entry
	// data for trunk,level1, are more branches
	// data for level2 is leaves
	// counter records how many active ch
	unks or branches there are
	//
	typedef struct t_Branch {
		void *data[16*16*8];
		int counter;
		
		struct t_Branch *parent;
		int slot;
	} Branch; 
	
	//--------------------------------------------------------------------
	typedef struct t_Leaf {
		Chunk *data[16*16*8];
		boost::uint8_t state[16*16*8];	// CHUNKSTATE_*
		boost::uint8_t locks[16*16*8];	// lock counter for shared locking
		int counter;

		int shared_lock_counter;
		int exclusive_lock_status;

		struct t_Branch *parent;
		int slot;
		struct t_Leaf *next;
		bool in_garbage_list;
	} Leaf;

	//--------------------------------------------------------------------

	Branch trunk;

	// list of leafs without active pointers (for memory cleanup)
	Leaf *inactive_leaf_first;
	Leaf *inactive_leaf_last;

	int leaf_garbage_memory_total;		// garbage memory counter (bytes)
	int leaf_garbage_memory_limit;		// threshold of when counter is over allowed limit (kb)
	int leaf_garbage_memory_cleaned;	// threshold of how much to clean (kb)
	//int leaf_cleanup_sleep_duration;
	int leaf_cleanup_throttle;			// during a clean, how many leaves are processed in between unlocking
										// to allow other threads to process
	volatile unsigned int leaf_cleanup_thread_active;

	boost::thread leaf_cleanup_thread;
	boost::mutex garbage_mutex;
	boost::condition_variable cvar_garbage_limit;

	//---------------------------------------------------------------------
	// threaded function to cleanup inactive leaves when they build up too much
	//
	void LeafCleanupThread();

	// add an inactive leaf to the garbage list
	void AddLeafToGarbage( Leaf *leaf );
	bool DeleteInactiveLeafE();

	void RunCleanup();

	//---------------------------------------------------------------------
	// create a [sub] branch/leaf
	//
	// the tree must be exlusively locked
	//
	Branch * CreateBranch( Branch *parent, int slot );
	Leaf * CreateLeaf( Branch *parent, int slot );

	//---------------------------------------------------------------------
	// delete a leaf and parent branches if they are empty
	//
	void DeleteLeaf( Leaf *leaf );

	//---------------------------------------------------------------------
	// delete a branch and parent branches if they are empty
	// (used by DeleteLeaf)
	//
	void DeleteBranch( Branch *branch );
	
	//---------------------------------------------------------------------
	// get the branch and slot index for an active (known) slot
	//
	// this must only be done if the caller has a shared or exclusive lock
	// on the *chunk*
	//
	void GetActiveLeafSlot( boost::uint64_t shuffled_coords, Leaf **r_leaf, int *r_slot );

	//---------------------------------------------------------------------
	// get the branch and slot index for a slot that may not exist
	// returns false if it doesn't
	//
	// this must be done with the tree locked
	// 
	bool GetLeafSlot( boost::uint64_t shuffled_coords, Leaf **r_leaf, int *r_slot );

public:

	enum {
		ACQUIREINACTIVE_SUCCESS,

		ACQUIREINACTIVE_NOTINACTIVE
	};

	enum {
		ACQUIREACTIVE_SUCCESS,

		ACQUIREACTIVE_NOTACTIVE,
		ACQUIREACTIVE_NOTACTIVE_EMPTY,
		ACQUIREACTIVE_ISLOCKED,
		ACQUIREACTIVE_UNKNOWNERROR
	};
	
	//--------------------------------------------------------------------
	// create chunk tree, specify max dimensions
	// dimensions must be multiple of 4 (region size)
	// horizontal dimensions maximum is 65536 (that's like 650 miles)
	// vertical dimension maximum is 4096 (that's like 40 miles)
	//
	ChunkTree( int garbage_kb_limit = 50000 );
	~ChunkTree();

	//--------------------------------------------------------------------
	// packed coordinate functions
	//
	boost::uint64_t PackCoords( int x, int y, int z );
	void UnpackCoords( boost::uint64_t packed, int &x, int &y, int &z );

	//--------------------------------------------------------------------
	// coordinate coding function
	boost::uint64_t ShuffleCoords( int x, int y, int z );
	 
	//---------------------------------------------------------------
	// try to acquire inactive slot
	// if the slot is active already, this function will return false
	//
	// if not, the slot will be set to LOCKED and the original inactive state
	// will be returned
	//
	// there is no blocking version !
	//
	int TryAcquireInactive( int x, int y, int z, int &original_state );
	int TryAcquireInactive( boost::uint64_t index, int &original_state );
	int TryAcquireInactiveS( boost::uint64_t shuffled_coords, int &original_state );

	//---------------------------------------------------------------
	// try to acquire active slot
	// if the slot is inactive, this function will return fase
	//
	// if not, the slot will be set to LOCKED and the caller will
	// assume exclusive access over the chunk and slot
	//
	// Try version will return immediately if the chunk is locked or inactive
	//
	// Wait version will return only if the chunk is or becomes inactive
	// and will wait to lock it otherwise (it can still fail if the chunk
	// becomes inactive while it is waiting)
	//
	// Shared suffix enables locking in shared mode, and only locks if the state is
	// READY or already SHARED
	//
	// on SUCCESS, the address is filled in with the chunk pointer
	//
	// E suffix assumes caller has already locked the tree
	//
	int TryAcquireActive( int x, int y, int z, Chunk **address );
	int TryAcquireActive( boost::uint64_t index, Chunk **address );
	int TryAcquireActiveS( boost::uint64_t shuffled_coords, Chunk **address );
	
	int TryAcquireActiveES( boost::uint64_t shuffled_coords, Chunk **address );

	int WaitAcquireActive( int x, int y, int z, Chunk **address );
	int WaitAcquireActive( boost::uint64_t index, Chunk **address );
	int WaitAcquireActiveS( boost::uint64_t shuffled_coords, Chunk **address );

	int TryAcquireActiveShared( int x, int y, int z, Chunk **address );
	int TryAcquireActiveShared( boost::uint64_t index, Chunk **address );
	int TryAcquireActiveSharedS( boost::uint64_t shuffled_coords, Chunk **address );
	
	int TryAcquireActiveSharedES( boost::uint64_t shuffled_coords, Chunk **address );

	int WaitAcquireActiveShared( int x, int y, int z, Chunk **address );
	int WaitAcquireActiveShared( boost::uint64_t index, Chunk **address );
	int WaitAcquireActiveSharedS( boost::uint64_t shuffled_coords, Chunk **address );

	//---------------------------------------------------------------
	// this is used on LOCKED (exclusive) slots only 
	// when the thread who is changing it is finished
	//
	// the state will be set to READY and the address is updated
	//
	void ReleaseSetPointer( int x, int y, int z, Chunk *address );
	void ReleaseSetPointer( boost::uint64_t index, Chunk *address );
	void ReleaseSetPointerS( boost::uint64_t shuffled_coords, Chunk *address );

	//---------------------------------------------------------------
	// release LOCKED (exclusive) slot, and set inactive state
	//
	// this is used for setting an inactive state or EMPTY active state
	// it also erases the address
	//
	void ReleaseSetInactive( int x, int y, int z, int inactive_state );
	void ReleaseSetInactive( boost::uint64_t index, int inactive_state );
	void ReleaseSetInactiveS( boost::uint64_t shuffled_coords, int inactive_state );

	//---------------------------------------------------------------
	// release SHARED slot
	//
	// only use after successful AcquireActiveShared call
	//
	void ReleaseShared( int x, int y, int z );
	void ReleaseShared( boost::uint64_t index );
	void ReleaseSharedS( boost::uint64_t shuffled_coords );

	//---------------------------------------------------------------
	// check the state of a slot
	//
	// this is just a hint, as the state can be unpredictably changed
	// when the function returns, because it does not place a lock
	// on the state
	//
	// E suffix assumes caller has already locked the tree
	//
	int QueryState( int x, int y, int z );
	int QueryState( boost::uint64_t index );
	int QueryStateS( boost::uint64_t shuffled_coords );

	int QueryStateE( int x, int y, int z );
	int QueryStateE( boost::uint64_t index );
	int QueryStateES( boost::uint64_t shuffled_coords );

};

}

#endif
