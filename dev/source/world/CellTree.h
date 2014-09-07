//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//

#pragma once

// the CELL Tree manages CELLS in a ginormous world using a tree structure
//
// 

// tree format:
// trunk: 16x16x8
// branch 1: 16x16x8
// branch 2: 16x16x8
// leaf: 16x16x8	// (max 650x650x40 miles (assuming 1 cube = 1 meter))

//------------------------------------------------------------------------------------------------------------------------
#include <boost/thread.hpp>
#include <boost/cstdint.hpp>
#include <boost/filesystem.hpp>
#include "world/Cell.h"

//------------------------------------------------------------------------------------------------------------------------
namespace World {

//------------------------------------------------------------------------------------------------------------------------
class CellTree {

	//-----------------------------------------------------------------------
	// mutex for locking the tree
	boost::mutex tree_mutex;

	// cvar that is notified when a lock on a cell or leaf is unlocked
	boost::condition_variable cvar_lock_changes;

	// cvar that is notified when a cell is finished being created
	boost::condition_variable cvar_creation;
	
	//-----------------------------------------------------------------------
	/// branch entry
	// data for trunk,level1 are more branches
	// data for level2 is leaves
	// counter records how many active branches or leaves it contains in the data
	//
	typedef struct t_Branch {
		void *data[16*16*8];	// data map
		int counter;			// active branches

		// parent and slot in parent
		struct t_Branch *parent;
		int slot;
	} Branch;

	enum {
		LEAF_XLOCK_OFF,
		LEAF_XLOCK_RESERVED,
		LEAF_XLOCK_ACTIVE
	};

	//-----------------------------------------------------------------------
	typedef struct t_Leaf {
		Cell *data[16*16*8];

		// cell state of data
		boost::uint8_t state[16*16*8];

		// shared lock counter
		boost::uint8_t locks[16*16*8];

		// if this is 0, this leaf is 'inactive'
		// this is the sum of ready cells (EXCLUDING EMPTY), and any kind of locks
		int total_references;

		// number of ready data entries
		// excluding "EMPTY" data entries
		//int ready_entries;

		// counter for cell exclusive locks
		// used to see if AcquireZone can execute
		int total_cell_xlocks;

		// counter for cell reserve locks
		// used to see if ReserveZone can execute
		int total_cell_reserves;

		// counter for cell shared locks
		// used to see if UpgradeZone can execute
		int total_cell_shares;

		int zone_shares;// number of shared locks on the leaf itself
		int zone_xlock; // LEAF_XLOCK_*

		// parent data
		struct t_Branch *parent;
		int slot;

		// linked list node used for memory management (garbage collection)
		struct t_Leaf *next;

		bool in_garbage_list;
	} Leaf;

	//-----------------------------------------------------------------------
	Branch trunk; // start of tree

	//-----------------------------------------------------------------------
	// list of leaves without active pointers (for memory cleanup)
	Leaf *inactive_leaf_list_first;
	Leaf *inactive_leaf_list_last;

	int leaf_garbage_memory_total;	// garbage memory counter (leafs)
	int leaf_garbage_memory_limit;	// limit of memory to maintain (leafs)
	int leaf_cleanup_throttle;		// how many leaves are deleted when memory exceeds the limit
	volatile unsigned int leaf_cleanup_thread_active;	// variable for shutting down leaf cleanup thread
	
	boost::thread leaf_cleanup_thread;	//
	//boost::mutex garbage_mutex;			// mutex for accessing garbage list and cvar
	boost::condition_variable cvar_garbage_limit;	// notified when memory total exceeds limit

	//-----------------------------------------------------------------------
	// statistical variable, counts all bytes of allocated memory
	//
	volatile unsigned int memory_usage_total;

	//-----------------------------------------------------------------------
	// threaded function to cleanup inactive leaves to maintain a memory limit
	//
	void LeafCleanupThread();

	// add an inactive leaf to the garbage list
	void AddLeafToGarbage( Leaf *leaf );

	// test reference counter and add leaf to garbage
	void GarbageTest( Leaf *leaf );
	
	// used by leaf cleanup
	bool DeleteInactiveLeafE();
	void RunCleanup();

	//-----------------------------------------------------------------------
	// create a [sub] branch/leaf
	// the tree must be locked during this op
	//
	Branch *CreateBranch( Branch *parent, int slot );
	Leaf *CreateLeaf( Branch *parent, int slot );

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
	// get the leaf and slot index for an active (known) cell slot
	//
	// this must only be done if the caller has a shared or exclusive lock
	// on the *cell* or the leaf (zone) itself
	//
	// it's not going anywhere when its locked, so this function is
	// optimized to skip checking for valid branches while navigating
	void GetActiveLeafSlot( boost::uint64_t shuffled_coords, Leaf **r_leaf, int *r_slot = 0 );
	
	//---------------------------------------------------------------------
	// get the leaf and cell slot index for a slot that may not exist
	// returns false if it doesn't
	//
	// these functions do not lock the tree and must be called within an existing lock
	// 
	bool GetLeafSlot( boost::uint64_t shuffled_coords, Leaf **r_leaf, int *r_slot );
	
	// same as above, but creates non existant entries
	void GetLeafSlotOrCreate( boost::uint64_t shuffled, Leaf **r_leaf, int *r_slot );
	
public:

	CellTree( int garbage_kb_limit = 50000 );
	~CellTree();

	//---------------------------------------------------------------------
	enum {
		RESULT_SUCCESS,	// lock successfully placed

		RESULT_NOTKNOWN,	// KNOWN lock tried on UNKNOWN cell/leaf
		RESULT_KNOWN,		// CREATION lock tried on KNOWN cell
		RESULT_CREATION_ACTIVE, // CREATION lock tried on active CREATION lock
		RESULT_NO_CREATION,	// WaitCreation tried on inactive cell
		
		RESULT_EXLOCKED,	// EXCLUSIVE (or reserve) lock is active and NOWAIT was specified
		RESULT_SHARED_ACTIVE,	// shared locks are active

		RESULT_TOOMANY,	// need to wait until some locks are released (NOWAIT was specified and lock counter maxed out)

		RESULT_INVALIDAREA,	// bad area arguments specified
	};

	//---------------------------------------------------------------------
	enum {
		FLAG_NOWAIT=1,		// return instantly if the target (or any target in a set) is exclusively locked
		FLAG_KNOWNONLY=2,	// do not lock unknown cells (ignored for zone locks)
		FLAG_TEST=4,		// don't actually lock, just test

		FLAG_ALREADYLOCKED=8	// to tell UpgradeAreaE that the EXCLUSIVE lock bit is already set
								
		//FLAG_REQUEST=4,		// request the cell if it isn't known
	};
	 
	//--------------------------------------------------------------------
	// get mutex for tree operations
	// use this to lock for complex operations
	//
	// when locking the tree manually, use the E suffix functions
	// otherwise it will deadlock
	//
	boost::mutex *GetMutex();

	//--------------------------------------------------------------------
	// coordinate coding function (for S suffix functions)
	static boost::uint64_t ShuffleCoords( int x, int y, int z );
	static boost::uint64_t ShuffleCoords( boost::uint64_t index );

	//---------------------------------------------------------------------
	// acquire creation lock on a cell
	// returns LOCK_SUCCESS on success and fills the original state reference
	// failure returns:
	//   LOCK_CREATION_ACTIVE - creation lock already in place
	//   LOCK_KNOWN - cell data is known and creation locks are illegal
	//
	int AcquireCreationSE( boost::uint64_t shuffled, int &original_state );
	int AcquireCreationS( boost::uint64_t shuffled, int &original_state );
	int AcquireCreation( int x, int y, int z, int &original_state );
	int AcquireCreation( boost::uint64_t index, int &original_state );

	//---------------------------------------------------------------------
	// release creation lock and set cell address
	//
	void ReleaseCreationSE( boost::uint64_t shuffled, Cell *address, bool empty );
	void ReleaseCreationS( boost::uint64_t shuffled, Cell *address, bool empty );
	void ReleaseCreation( int x, int y, int z, Cell *address, bool empty );
	void ReleaseCreation( boost::uint64_t index, Cell *address, bool empty );

	//---------------------------------------------------------------------
	// place a shared lock on a cell
	// 
	//  addresses: filled in with cell address (if applicable)
	//  state: filled in with cell state
	//  flags: flags.
	// Zone: lock entire zone of cells
	//  a zone is a 16x16x8 area of cells (a leaf)
	//
	// zone parameters target the zone that the address point to (the leaf slot is ignored)
	//
	int AcquireSE( boost::uint64_t shuffled, Cell **address, int *state, boost::unique_lock<boost::mutex> &lock, int flags = 0 );
	int AcquireS( boost::uint64_t shuffled, Cell **address, int *state, int flags = 0 );
	int Acquire( int x, int y, int z, Cell **address, int *state, int flags = 0 );
	int Acquire( boost::uint64_t index, Cell **address, int *state, int flags = 0 );
	int AcquireAreaE( const int *pos1, const int *pos2, boost::unique_lock<boost::mutex> &lock, int flags = 0 );
	int AcquireArea( int x1, int y1, int z1, int x2, int y2, int z2, int flags = 0 );
	int AcquireArea( int x, int y, int z, int size, int flags = 0 );
	int AcquireArea( const int *pos1, const int *pos2, int flags = 0 );
	int AcquireArea( const int *pos, int size, int flags = 0 );
	int AcquireZoneSE( boost::uint64_t shuffled, boost::unique_lock<boost::mutex> &lock, int flags = 0 );
	int AcquireZoneS( boost::uint64_t shuffled, int flags = 0 );
	int AcquireZone( int x, int y, int z, int flags = 0 );
	int AcquireZone( boost::uint64_t index, int flags = 0 );
	int AcquireZoneAreaE( const int *pos1, const int *pos2, boost::unique_lock<boost::mutex> &lock, int flags = 0 );
	int AcquireZoneArea( int x1, int y1, int z1, int x2, int y2, int z2, int flags = 0 );
	int AcquireZoneArea( int x1, int y1, int z1, int size, int flags = 0 );
	int AcquireZoneArea( const int *pos1, const int *pos2, int flags = 0 );
	int AcquireZoneArea( const int *pos, int size, int flags = 0 );

	//---------------------------------------------------------------------
	// functions to test if a lock is available REPLACED BY FLAG_TEST
	// 
	// use these with a lock on the mutex already
	// (for safe multi-area operations)
	//
	// (these fail if there is an exclusive lock)
	//
	/*
	bool CanAcquireSE( boost::uint64_t shuffled );
	bool CanAcquireAreaE( const int *pos1, const int *pos2 );
	bool CanAcquireZoneSE( boost::uint64_t shuffled );
	bool CanAcquireZoneAreaE( const int *pos1, const int *pos2 );
	*/
	//---------------------------------------------------------------------
	// release shared lock on cell
	//
	void ReleaseSE( boost::uint64_t shuffled );
	void ReleaseS( boost::uint64_t shuffled );
	void Release( int x, int y, int z );
	void Release( boost::uint64_t index );
	void ReleaseAreaE( const int *pos1, const int *pos2 );
	void ReleaseArea( int x1, int y1, int z1, int x2, int y2, int z2 );
	void ReleaseArea( int x, int y, int z, int size );
	void ReleaseArea( const int *pos1, const int *pos2 );
	void ReleaseArea( const int *pos, int size );
	void ReleaseZoneSE( boost::uint64_t shuffled );
	void ReleaseZoneS( boost::uint64_t shuffled );
	void ReleaseZone( int x, int y, int z );
	void ReleaseZone( boost::uint64_t index );
	void ReleaseZoneAreaE( const int *pos1, const int *pos2 );
	void ReleaseZoneArea( int x1, int y1, int z1, int x2, int y2, int z2 );
	void ReleaseZoneArea( int x, int y, int z, int size );
	void ReleaseZoneArea( const int *pos1, const int *pos2 );
	void ReleaseZoneArea( const int *pos, int size );

	//---------------------------------------------------------------------
	// reserve lock on a cell
	//
	// the reserve lock is a shared lock that can be upgraded into an
	// exclusive lock
	//
	// only one reserve lock can exist on a cell at a time
	//
	int ReserveSE( boost::uint64_t shuffled, boost::unique_lock<boost::mutex> &lock, int flags = 0 );
	int ReserveS( boost::uint64_t shuffled, int flags = 0 );
	int Reserve( int x, int y, int z, int flags = 0 );
	int Reserve( boost::uint64_t index, int flags = 0 );
	int ReserveAreaE( const int *pos1, const int *pos2, boost::unique_lock<boost::mutex> &lock, int flags = 0 );
	int ReserveArea( int x1, int y1, int z1, int x2, int y2, int z2, int flags = 0 );
	int ReserveArea( int x, int y, int z, int size, int flags = 0 );
	int ReserveArea( const int *pos1, const int *pos2, int flags = 0 );
	int ReserveArea( const int *pos, int size, int flags = 0 );
	int ReserveZoneSE( boost::uint64_t shuffled, boost::unique_lock<boost::mutex> &lock, int flags = 0 );
	int ReserveZoneS( boost::uint64_t shuffled, int flags = 0 );
	int ReserveZone( int x, int y, int z, int flags = 0 );
	int ReserveZone( boost::uint64_t index, int flags = 0 );
	int ReserveZoneAreaE( const int *pos1, const int *pos2, boost::unique_lock<boost::mutex> &lock, int flags = 0 );
	int ReserveZoneArea( int x1, int y1, int z1, int x2, int y2, int z2, int flags = 0 );
	int ReserveZoneArea( int x, int y, int z, int size, int flags = 0 );
	int ReserveZoneArea( const int *pos1, const int *pos2, int flags = 0 );
	int ReserveZoneArea( const int *pos, int size, int flags = 0 );

	//---------------------------------------------------------------------
	// lock tests for reserve REPLACED WITH FLAG_TEST
	//
	/*
	bool CanReserveSE( boost::uint64_t shuffled );
	bool CanReserveAreaE( const int *pos1, const int *pos2 );
	bool CanReserveZoneSE( boost::uint64_t shuffled );
	bool CanReserveZoneAreaE( const int *pos1, const int *pos2 );
	*/
	//---------------------------------------------------------------------
	// activate reserved lock
	// which gives exclusive access to the cell for changes
	//
	// this blocks until the lock takes place
	//
	// any new shared access is blocked until the exclusive lock 
	// is released vis Complete
	//
	// using NOWAIT with upgrade will cause it to set the lock flag and 
	// return immediately if a shared lock is still active on the data
	// call Upgrade until you get ACQUIRE_SUCCESS if you are not using
	// the blocking version
	//
	int UpgradeSE( boost::uint64_t shuffled, boost::unique_lock<boost::mutex> &lock, int flags = 0 );
	int UpgradeS( boost::uint64_t shuffled, int flags = 0 );
	int Upgrade( int x, int y, int z, int flags = 0 );
	int Upgrade( boost::uint64_t index, int flags = 0 );
	int UpgradeAreaE( const int *pos1, const int *pos2, boost::unique_lock<boost::mutex> &lock, int flags = 0 );
	int UpgradeArea( int x1, int y1, int z1, int x2, int y2, int z2, int flags = 0 );
	int UpgradeArea( int x, int y, int z, int size, int flags = 0 );
	int UpgradeArea( const int *pos1, const int *pos2, int flags = 0 );
	int UpgradeArea( const int *pos, int size, int flags = 0 );
	int UpgradeZoneSE( boost::uint64_t shuffled, boost::unique_lock<boost::mutex> &lock, int flags = 0 );
	int UpgradeZoneS( boost::uint64_t shuffled, int flags = 0 );
	int UpgradeZone( int x, int y, int z, int flags = 0 );
	int UpgradeZone( boost::uint64_t index, int flags = 0 );
	int UpgradeZoneAreaE( const int *pos1, const int *pos2, boost::unique_lock<boost::mutex> &lock, int flags = 0 );
	int UpgradeZoneArea( int x1, int y1, int z1, int x2, int y2, int z2, int flags = 0 );
	int UpgradeZoneArea( int x, int y, int z, int size, int flags = 0 );
	int UpgradeZoneArea( const int *pos1, const int *pos2, int flags = 0 );
	int UpgradeZoneArea( const int *pos, int size, int flags = 0 );

	//---------------------------------------------------------------------
	// release an exclusive or reserve lock
	//
	void CompleteSE( boost::uint64_t shuffled, int flags = 0 );
	void CompleteS( boost::uint64_t shuffled, int flags = 0 );
	void Complete( int x, int y, int z, int flags = 0 );
	void Complete( boost::uint64_t index, int flags = 0 );
	void CompleteAreaE( const int *pos1, const int *pos2, int flags = 0 );
	void CompleteArea( int x1, int y1, int z1, int x2, int y2, int z2, int flags = 0 );
	void CompleteArea( int x, int y, int z, int size, int flags = 0 );
	void CompleteArea( const int *pos1, const int *pos2, int flags = 0 );
	void CompleteArea( const int *pos, int size, int flags = 0 );
	void CompleteZoneSE( boost::uint64_t shuffled, int flags = 0 );
	void CompleteZoneS( boost::uint64_t shuffled, int flags = 0 );
	void CompleteZone( int x, int y, int z, int flags = 0 );
	void CompleteZone( boost::uint64_t index, int flags = 0 );
	void CompleteZoneAreaE( const int *pos1, const int *pos2, int flags = 0 );
	void CompleteZoneArea( int x1, int y1, int z1, int x2, int y2, int z2, int flags = 0 );
	void CompleteZoneArea( int x, int y, int z, int size, int flags = 0 );
	void CompleteZoneArea( const int *pos1, const int *pos2, int flags = 0 );
	void CompleteZoneArea( const int *pos, int size, int flags = 0 );
	
	//---------------------------------------------------------------------
	// wait until a cell or area of cells is created
	//
	// ideally done with a lock on the cell or area
	//
	int WaitCreationSE( boost::uint64_t shuffled, boost::unique_lock<boost::mutex> &lock );
	int WaitCreationS( boost::uint64_t shuffled );
	int WaitCreation( int x, int y, int z );
	int WaitCreation( boost::uint64_t index );
	int WaitCreationAreaE( const int *pos1, const int *pos2, boost::unique_lock<boost::mutex> &lock );
	int WaitCreationArea( int x1, int y1, int z1, int x2, int y2, int z2 );
	int WaitCreationArea( int x, int y, int z, int size );
	int WaitCreationArea( const int *pos1, const int *pos2 );
	int WaitCreationArea( const int *pos, int size );
	
	bool IsCreatedSE( boost::uint64_t shuffled );
	bool IsCreatedS( boost::uint64_t shuffled );
	bool IsCreated( int x, int y, int z );
	bool IsCreated( boost::uint64_t index );

	bool IsAreaCreatedE( const int *pos1, const int *pos2 );
	bool IsAreaCreated( int x1, int y1, int z1, int x2, int y2, int z2 );
	bool IsAreaCreated( int x, int y, int z, int size );
	bool IsAreaCreated( const int *pos1, const int *pos2 );
	bool IsAreaCreated( const int *pos, int size );

	//---------------------------------------------------------------------
	// read cell state
	// the state may change at any time without a lock
	//
	void GetCellStateSE( boost::uint64_t shuffled, int *state, Cell **address );
	void GetCellStateS( boost::uint64_t shuffled, int *state, Cell **address );
	void GetCellState( int x, int y, int z, int *state, Cell **address );
	void GetCellState( boost::uint64_t index, int *state, Cell **address );

	//---------------------------------------------------------------------
	// check if a cell is locked (either shared or reserved)
	//
	bool IsLockedSE( boost::uint64_t shuffled );
	bool IsLockedS( boost::uint64_t shuffled );
	bool IsLocked( int x, int y, int z );
	bool IsLocked( boost::uint64_t index );
	
	//---------------------------------------------------------------------
	// change cell state
	// to be used ONLY during creation with the owner of the creation lock
	// or by the owner of an exclusive lock
	//
	// set state to -1 to not change it
	//
	// omit cell address to not change it
	//
	void SetCellStateSE( boost::uint64_t shuffled, int state, Cell *address );
	void SetCellStateS( boost::uint64_t shuffled, int state, Cell *address );
	void SetCellState( int x, int y, int z, int state, Cell *address );
	void SetCellState( boost::uint64_t index, int state, Cell *address );

	void SetCellStateSE( boost::uint64_t shuffled, int state );
	void SetCellStateS( boost::uint64_t shuffled, int state );
	void SetCellState( int x, int y, int z, int state );
	void SetCellState( boost::uint64_t index, int state );

	//---------------------------------------------------------------------
	//
	void SetUnknownCellStateSE( boost::uint64_t shuffled, int state );
	void SetUnknownCellStateS( boost::uint64_t shuffled, int state );
	void SetUnknownCellState( int x, int y, int z, int state );
	void SetUnknownCellState( boost::uint64_t index, int state );

	//---------------------------------------------------------------------
	// function to quickly sample a cube in a cell
	//
	// parameters are in cube units
	//
	// returns -1 if cube doesnt exist
	// and -2 if cube happened to be exclusively locked at this moment
	//
	int SampleE( int x, int y, int z );
	int Sample( int x, int y, int z );
};

}
