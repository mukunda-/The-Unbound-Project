//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//

//-------------------------------------------------------------------------------------------------
#include "stdafx.h"

//-------------------------------------------------------------------------------------------------
namespace World {

namespace {

//-------------------------------------------------------------------------------------------------
void SetupArea( int *pos1, int *pos2, int x1, int y1, int z1, int x2, int y2, int z2 ) {
	pos1[0] = x1;
	pos1[1] = y1;
	pos1[2] = z1;
	pos2[0] = x2;
	pos2[1] = y2;
	pos2[2] = z2;
}

//-------------------------------------------------------------------------------------------------
void SetupArea( int *pos1, int *pos2, int x, int y, int z, int size ) {
	pos1[0] = x - size;
	pos1[1] = y - size;
	pos1[2] = z - size;
	pos2[0] = x + size;
	pos2[1] = y + size;
	pos2[2] = z + size;
}

//-------------------------------------------------------------------------------------------------
void SetupArea( int *pos1, int *pos2, const int *pos, int size ) {
	for( int i = 0; i < 3; i++ ) {
		pos1[i] = pos[i] - size;
		pos2[i] = pos[i] + size;
	}
}

}

//-------------------------------------------------------------------------------------------------
void CellTree::LeafCleanupThread() {
	boost::unique_lock<boost::mutex> lock(tree_mutex);

	while( true ) {
		if( !leaf_cleanup_thread_active ) break;

		if( leaf_garbage_memory_total <= leaf_garbage_memory_limit ) {
			cvar_garbage_limit.wait(lock);
		} else {
			RunCleanup();
			if( leaf_garbage_memory_total > leaf_garbage_memory_limit ) {
				lock.unlock();
				boost::this_thread::yield();
				lock.lock();
			}
		}
	}
}

//-------------------------------------------------------------------------------------------------
void CellTree::AddLeafToGarbage( Leaf *leaf ) {
	//boost::lock_guard<boost::mutex> lock(garbage_mutex);
	if( leaf->in_garbage_list ) {
		return;
	}

	leaf->in_garbage_list = true;
	leaf->next = 0;

	if( inactive_leaf_list_last ) {
		inactive_leaf_list_last->next = leaf;
		inactive_leaf_list_last = leaf;
	} else {
		inactive_leaf_list_first = inactive_leaf_list_last = leaf;
	}

	leaf_garbage_memory_total += sizeof( Leaf );
	if( leaf_garbage_memory_total > leaf_garbage_memory_limit ) {
		cvar_garbage_limit.notify_all();
	}
}

//-------------------------------------------------------------------------------------------------
void CellTree::GarbageTest( Leaf *leaf ) {
	if( leaf->total_references != 0 ) return;
	
	AddLeafToGarbage( leaf );
}

//-------------------------------------------------------------------------------------------------
bool CellTree::DeleteInactiveLeafE() {
	if( !inactive_leaf_list_first ) return false;

	Leaf *leaf = inactive_leaf_list_first;
	inactive_leaf_list_first = inactive_leaf_list_first->next;
	if( !inactive_leaf_list_first ) inactive_leaf_list_last = 0;

	leaf->in_garbage_list = false;
	leaf_garbage_memory_total--;

	if( leaf->total_references == 0 ) {

		DeleteLeaf( leaf );
	}
	return true;
}

//-------------------------------------------------------------------------------------------------
void CellTree::RunCleanup() {
	boost::lock_guard<boost::mutex> lock(tree_mutex);
	for( int counter = 0; counter < leaf_cleanup_throttle; counter++ ) {
		if( !DeleteInactiveLeafE() ) break;
	}
}

//-------------------------------------------------------------------------------------------------
CellTree::Branch *CellTree::CreateBranch( Branch *parent, int slot ) {
	if( parent->data[slot] != 0 ) return (Branch*)parent->data[slot];
	Branch *b = (Branch*)Memory::AllocMem( sizeof(Branch) );
	memset( b, 0, sizeof(b) );
	b->parent = parent;
	b->slot = slot;
	parent->data[slot] = b;
	parent->counter++;
	return b;
}

//-------------------------------------------------------------------------------------------------
CellTree::Leaf *CellTree::CreateLeaf( Branch *parent, int slot ) {
	if( parent->data[slot] != 0 ) return (Leaf*)parent->data[slot];
	Leaf *b = (Leaf*)Memory::AllocMem( sizeof(Leaf) );
	memset( b, 0, sizeof(b) );
	b->parent = parent;
	b->slot = slot;
	parent->data[slot] = b;
	parent->counter++;
	return b;
}

//-------------------------------------------------------------------------------------------------
void CellTree::DeleteBranch( Branch *branch ) {
	if( branch == &trunk ) return;
	assert( branch->counter == 0 );
	Branch *parent = branch->parent;
	int slot = branch->slot;
	Memory::FreeMem( branch );
	if( parent ) {
		parent->data[slot] = 0;
		parent->counter--;
		if( parent->counter == 0 ) {
			DeleteBranch( parent );
		}
	}
}

//-------------------------------------------------------------------------------------------------
void CellTree::DeleteLeaf( Leaf *leaf ) {

	// make sure the leaf is not being used
	assert( leaf->total_references == 0 );
	//assert( leaf->zone_xlock == 0 );
	//assert( leaf->zone_shares == 0 );
	//assert( leaf->total_locks == 0 );

	Branch *parent = leaf->parent;
	int slot = leaf->slot;
	Memory::FreeMem( leaf );
	if( parent ) {
		parent->data[slot] = 0;
		parent->counter--;
		if( parent->counter == 0 ) {
			DeleteBranch( parent );
		}
	}
}

//-------------------------------------------------------------------------------------------------
void CellTree::GetActiveLeafSlot( boost::uint64_t shuffled_coords, Leaf **r_leaf, int *r_slot ) {
	Branch *branch;

	branch = (Branch*)trunk.data[shuffled_coords>>44];
	assert( branch );
	branch = (Branch*)branch->data[(shuffled_coords>>32)&2047];
	assert( branch );
	Leaf *leaf = (Leaf*)branch->data[(shuffled_coords>>12)&2047];
	assert( leaf );
	int addr = shuffled_coords&2047;

	if( r_leaf ) {
		*r_leaf = leaf;
	}

	if( r_slot )  {
		*r_slot = addr;
	}
}


//-------------------------------------------------------------------------------------------------
bool CellTree::GetLeafSlot( boost::uint64_t shuffled_coords, Leaf **r_leaf, int *r_slot ) {
	Branch *branch = (Branch*)trunk.data[shuffled_coords>>44];
	if( !branch ) return false;
	branch = (Branch*)branch->data[(shuffled_coords>>32)&2047];
	if( !branch ) return false;
	Leaf *leaf = (Leaf*)branch->data[(shuffled_coords>>12)&2047];
	if( !leaf ) return false;
	int addr = shuffled_coords&2047;

	if( r_leaf ) {
		*r_leaf = leaf;
	}

	if( r_slot ) {
		*r_slot = addr;
	}
	return true;
}

//-------------------------------------------------------------------------------------------------
void CellTree::GetLeafSlotOrCreate( boost::uint64_t shuffled, Leaf **r_leaf, int *r_slot ) {
	Branch *branch;
	branch = CreateBranch( &trunk, shuffled>>44 );
	branch = CreateBranch( branch, (shuffled>>32)&2047 );
	Leaf *leaf = CreateLeaf( branch, (shuffled>>12)&2047 );
	

	if( r_leaf ) {
		*r_leaf = leaf;
	}

	if( r_slot ) {
		*r_slot = shuffled&2047;
	}

}


//-------------------------------------------------------------------------------------------------
CellTree::CellTree( int garbage_kb_limit ) {
	leaf_garbage_memory_total = 0;
	leaf_garbage_memory_limit = (garbage_kb_limit * 1000) / sizeof(Leaf);
	leaf_cleanup_throttle = 5;

	inactive_leaf_list_first = 0;
	inactive_leaf_list_last = 0;

	memset( &trunk, 0, sizeof(trunk) );


	InterlockedExchange( &memory_usage_total, 0 );
	InterlockedExchange( &leaf_cleanup_thread_active, 1 );

	leaf_cleanup_thread = boost::thread( boost::bind( &CellTree::LeafCleanupThread, this ) );

}

//-------------------------------------------------------------------------------------------------
CellTree::~CellTree() {
	InterlockedExchange( &leaf_cleanup_thread_active, 0 );
	leaf_cleanup_thread.join();

	// todo: free the rest of the tree
}


//-------------------------------------------------------------------------------------------------
boost::mutex *CellTree::GetMutex() {
	return &tree_mutex;
}

//-------------------------------------------------------------------------------------------------
boost::uint64_t CellTree::ShuffleCoords( int x, int y, int z ) {
	return ((boost::uint64_t)(((x&15) + (z&15)*16 + (y&7)*16))) +
	       ((boost::uint64_t)(((x>>4)&15)+((z>>4)&15)*16+((y>>3)&7)*256)<<12) +
	       ((boost::uint64_t)(((x>>8)&15)+((z>>8)&15)*16+((y>>6)&7)*256)<<32) +
	       ((boost::uint64_t)(((x>>12)+(z>>12)*16+(y>>9)*256))<<44);
}

//-------------------------------------------------------------------------------------------------
boost::uint64_t CellTree::ShuffleCoords( boost::uint64_t index ) {
	int x, y, z;
	UnpackCoords( index, x, y, z );
	return ShuffleCoords( x, y, z );
}

//-------------------------------------------------------------------------------------------------
int CellTree::AcquireCreationSE( boost::uint64_t shuffled, int &original_state ) {
	
	Leaf *leaf;
	int slot;
	GetLeafSlotOrCreate( shuffled, &leaf, &slot );
	//Branch *branch;
	//branch = CreateBranch( &trunk, shuffled >> 44 );
	//branch = CreateBranch( branch, (shuffled>>32) & 2047 );
	//Leaf *leaf = CreateLeaf( branch, (shuffled>>12) & 2047 );

	int state = leaf->state[slot];// (shuffled) & 2047 ];
	if( CellStateReady(state) ) {
		return RESULT_KNOWN;
	}

	if( CellStateCreation(state) ) {
		return RESULT_CREATION_ACTIVE;
	}

	leaf->state[slot] |= CELLSTATE_CREATION;
	leaf->total_references++;
	
	original_state = state;
	return RESULT_SUCCESS;
}

//-------------------------------------------------------------------------------------------------
int CellTree::AcquireCreationS( boost::uint64_t shuffled, int &original_state ) {
	boost::lock_guard<boost::mutex> lock(tree_mutex);
	return AcquireCreationSE( shuffled, original_state );
}

//-------------------------------------------------------------------------------------------------
int CellTree::AcquireCreation( int x, int y, int z, int &original_state ) {
	return AcquireCreationS( ShuffleCoords( x, y, z ), original_state );
}

//-------------------------------------------------------------------------------------------------
int CellTree::AcquireCreation( boost::uint64_t index, int &original_state ) {
	return AcquireCreationS( ShuffleCoords( index ), original_state );
}

//-------------------------------------------------------------------------------------------------
void CellTree::ReleaseCreationSE( boost::uint64_t shuffled, Cell *address, bool empty ) {
	Leaf *leaf;
	int slot;
	GetActiveLeafSlot( shuffled, &leaf, &slot );

	if( empty ) {
		leaf->total_references--;
		GarbageTest( leaf );
	}
	
	leaf->data[slot] = address;
	leaf->state[slot] &= ~CELLSTATE_CREATION;
	leaf->state[slot] |= CELLSTATE_READY + (empty ? CELLSTATE_EMPTY : 0);
	
	cvar_creation.notify_all();
}

//-------------------------------------------------------------------------------------------------
void CellTree::ReleaseCreationS( boost::uint64_t shuffled, Cell *address, bool empty ) {
	boost::lock_guard<boost::mutex> lock(tree_mutex);
	ReleaseCreationSE( shuffled, address, empty );
}/*
//-------------------------------------------------------------------------------------------------
void CellTree::ReleaseCreationS( boost::uint64_t shuffled, Cell *address, bool empty ) {
	ReleaseCreationSE( 
}*/

//-------------------------------------------------------------------------------------------------
void CellTree::ReleaseCreation( int x, int y, int z, Cell *address, bool empty ) {
	ReleaseCreationS( ShuffleCoords( x, y, z ), address, empty );
}

//-------------------------------------------------------------------------------------------------
void CellTree::ReleaseCreation( boost::uint64_t index, Cell *address, bool empty ) {
	ReleaseCreationS( ShuffleCoords( index ), address, empty );
}

//-------------------------------------------------------------------------------------------------
int CellTree::AcquireSE( boost::uint64_t shuffled, Cell **address, int *state, boost::unique_lock<boost::mutex> &lock, int flags ) {
	//boost::unique_lock<boost::mutex> lock(tree_mutex);
	int result;
	while( true ) {
		int slot;
		Leaf *leaf;

		if( flags & FLAG_KNOWNONLY ) {
			bool exists = GetLeafSlot( shuffled, &leaf, &slot );
			if( !exists ) {
				return RESULT_NOTKNOWN;
			}
		} else {
			GetLeafSlotOrCreate( shuffled, &leaf, &slot );
		}
		
		if( leaf->zone_xlock == LEAF_XLOCK_ACTIVE ) {
			if( flags & FLAG_NOWAIT ) {
				return RESULT_EXLOCKED;
			} else {
				cvar_lock_changes.wait( lock );
				continue;
			}
		}
		int state = leaf->state[slot];
		
		if( (!(state&CELLSTATE_READY)) && (flags & FLAG_KNOWNONLY) ) {
			return RESULT_NOTKNOWN;
		}
		
		if( state & CELLSTATE_LOCKED ) {
			if( flags & FLAG_NOWAIT ) {
				return RESULT_EXLOCKED;
			} else {
				cvar_lock_changes.wait( lock );
				continue;
			}
		}

		if( leaf->locks[slot] == 255 ) {
			if( flags & FLAG_NOWAIT ) {
				return RESULT_TOOMANY;
			} else {
				cvar_lock_changes.wait( lock );
				continue;
			}
		}

		if( flags & FLAG_TEST ) {
			return RESULT_SUCCESS;
		}

		leaf->locks[slot]++;
		leaf->total_references++;
		leaf->total_cell_shares++;
		
		return RESULT_SUCCESS;
	}
}

//-------------------------------------------------------------------------------------------------
int CellTree::AcquireS( boost::uint64_t shuffled, Cell **address, int *state, int flags ) {
	boost::unique_lock<boost::mutex> lock(tree_mutex);
	return AcquireSE( shuffled, address, state, lock, flags );
}

//-------------------------------------------------------------------------------------------------
int CellTree::Acquire( int x, int y, int z, Cell **address, int *state, int flags ) {
	return AcquireS( ShuffleCoords( x, y, z ), address, state, flags );
}

//-------------------------------------------------------------------------------------------------
int CellTree::Acquire( boost::uint64_t index, Cell **address, int *state, int flags ) {
	return AcquireS( ShuffleCoords( index ), address, state, flags );
}

//-------------------------------------------------------------------------------------------------
int CellTree::AcquireAreaE( const int *pos1, const int *pos2, boost::unique_lock<boost::mutex> &lock, int flags ) {

	int cpos1[3],cpos2[3];
	for( int i = 0; i < 3; i++ ) {
		cpos1[i] = pos1[i] >> 4;
		cpos2[i] = pos2[i] >> 4;
	}

	while( true ) {
		
		int test_result = RESULT_INVALIDAREA;
		for( int y = cpos1[1]; y <= cpos2[1]; y++ ) {
			for( int z = cpos1[2]; z <= cpos2[2]; z++ ) {
				for( int x = cpos1[0]; x <= cpos2[0]; x++ ) {
					test_result = AcquireSE( ShuffleCoords(x, y, z), 0, 0, lock, flags|FLAG_TEST );

					if( test_result != RESULT_SUCCESS )
						goto breakloop;
				}
			}
		}
		breakloop:
		if( test_result != RESULT_SUCCESS ) {
			if( test_result == RESULT_INVALIDAREA || test_result == RESULT_NOTKNOWN ) {
				return test_result;
			}
			if( (flags & FLAG_NOWAIT) && (test_result == RESULT_EXLOCKED || test_result == RESULT_TOOMANY) ) {
				return test_result;
			}
			
			if( !(flags & FLAG_TEST) ) {
				cvar_lock_changes.wait( lock );
				continue;
			} else {
				return test_result;
			}
		}

		if( flags & FLAG_TEST ) {
			return RESULT_SUCCESS;
		}

		for( int y = cpos1[1]; y <= cpos2[1]; y++ ) {
			for( int z = cpos1[2]; z <= cpos2[2]; z++ ) {
				for( int x = cpos1[0]; x <= cpos2[0]; x++ ) {
					AcquireSE( ShuffleCoords(x, y, z), 0, 0, lock, flags );
				}
			}
		}

		return RESULT_SUCCESS;
	}
}

//-------------------------------------------------------------------------------------------------
int CellTree::AcquireArea( int x1, int y1, int z1, int x2, int y2, int z2, int flags ) {
	
	int pos1[3], pos2[3];
	SetupArea( pos1, pos2, x1, y1, z1, x2, y2, z2 );

	{
		boost::unique_lock<boost::mutex> lock(tree_mutex);
		return AcquireAreaE( pos1, pos2, lock, flags );
	}
}

//-------------------------------------------------------------------------------------------------
int CellTree::AcquireArea( int x, int y, int z, int size, int flags ) {
	int pos1[3], pos2[3];
	SetupArea( pos1, pos2, x, y, z, size );
	
	return AcquireArea( pos1, pos2, flags );
}

//-------------------------------------------------------------------------------------------------
int CellTree::AcquireArea( const int *pos1, const int *pos2, int flags ) {
	boost::unique_lock<boost::mutex> lock(tree_mutex);
	return AcquireAreaE( pos1, pos2, lock, flags );
}

//-------------------------------------------------------------------------------------------------
int CellTree::AcquireArea( const int *pos, int size, int flags ) {
	int pos1[3], pos2[3];
	SetupArea( pos1, pos2, pos, size );
	return AcquireArea( pos1, pos2, flags );
}

//-------------------------------------------------------------------------------------------------
int CellTree::AcquireZoneSE( boost::uint64_t shuffled, boost::unique_lock<boost::mutex> &lock, int flags ) {
	
	int result;
	while( true ) {
		Leaf *leaf;

		GetLeafSlotOrCreate( shuffled, &leaf, 0 );

		if( leaf->zone_xlock == LEAF_XLOCK_ACTIVE || leaf->total_cell_xlocks ) {
			if( flags & FLAG_NOWAIT ) {
				return RESULT_EXLOCKED;
			} else {
				cvar_lock_changes.wait(lock);
				continue;
			}
		}

		if( flags & FLAG_TEST ) {
			return RESULT_SUCCESS;
		}

		leaf->zone_shares++;
		leaf->total_references++;
		return RESULT_SUCCESS;
	}
}

//-------------------------------------------------------------------------------------------------
int CellTree::AcquireZoneS( boost::uint64_t shuffled, int flags ) {
	boost::unique_lock<boost::mutex> lock(tree_mutex);
	return AcquireZoneSE( shuffled, lock, flags );
}

//-------------------------------------------------------------------------------------------------
int CellTree::AcquireZone( int x, int y, int z, int flags ) {
	return AcquireZoneS( ShuffleCoords( x, y, z ), flags );
}

//-------------------------------------------------------------------------------------------------
int CellTree::AcquireZone( boost::uint64_t index, int flags ) {
	return AcquireZoneS( ShuffleCoords( index ), flags );
}

//-------------------------------------------------------------------------------------------------
int CellTree::AcquireZoneAreaE( const int *pos1, const int *pos2, boost::unique_lock<boost::mutex> &lock, int flags ) {
	int cpos1[3], cpos2[3];
	cpos1[0] = pos1[0] >> 8;
	cpos1[1] = pos1[1] >> 8;
	cpos1[2] = pos1[2] >> 7;

	cpos2[0] = pos2[0] >> 8;
	cpos2[1] = pos2[1] >> 8;
	cpos2[2] = pos2[2] >> 7;

	while( true ) {

		int test_result = RESULT_INVALIDAREA;
		for( int y = cpos1[1]; y <= cpos2[1]; y++ ) {
			for( int z = cpos1[2]; z <= cpos2[2]; z++ ) {
				for( int x = cpos1[0]; x <= cpos2[0]; x++ ) {
					test_result = AcquireZoneSE( ShuffleCoords(x, y, z), lock, flags|FLAG_TEST );

					if( test_result != RESULT_SUCCESS )
						goto breakloop;
				}
			}
		}
		breakloop:
		if( test_result != RESULT_SUCCESS ) {
			if( test_result == RESULT_INVALIDAREA ) {
				return test_result;
			}
			if( (flags & FLAG_NOWAIT) && (test_result == RESULT_EXLOCKED || test_result == RESULT_TOOMANY) ) {
				return test_result;
			}

			if( !(flags & FLAG_TEST) ) {
				cvar_lock_changes.wait(lock);
				continue;
			}
		}

		if( flags & FLAG_TEST ) {
			return RESULT_SUCCESS;
		}

		for( int y = cpos1[1]; y <= cpos2[1]; y++ ) {
			for( int z = cpos1[2]; z <= cpos2[2]; z++ ) {
				for( int x = cpos1[0]; x <= cpos2[0]; x++ ) {
					AcquireZoneSE( ShuffleCoords(x, y, z), lock, flags );
				}
			}
		} 
		return RESULT_SUCCESS;
	}
}

//-------------------------------------------------------------------------------------------------
int CellTree::AcquireZoneArea( int x1, int y1, int z1, int x2, int y2, int z2, int flags ) {
	int pos1[3], pos2[3];
	SetupArea( pos1, pos2, x1, y1, z1, x2, y2, z2 );
	return AcquireZoneArea( pos1, pos2, flags );
}

//-------------------------------------------------------------------------------------------------
int CellTree::AcquireZoneArea( int x, int y, int z, int size, int flags ) {
	int pos1[3], pos2[3];
	SetupArea( pos1, pos2, x, y, z, size );
	return AcquireZoneArea( pos1, pos2, flags );
}

//-------------------------------------------------------------------------------------------------
int CellTree::AcquireZoneArea( const int *pos1, const int *pos2, int flags ) {
	boost::unique_lock<boost::mutex> lock(tree_mutex);
	return AcquireZoneAreaE( pos1, pos2, lock, flags );
}

//-------------------------------------------------------------------------------------------------
int CellTree::AcquireZoneArea( const int *pos, int size, int flags ) {
	int pos1[3], pos2[3];
	for( int i = 0; i < 3; i++ ) {
		pos1[i] = pos[i] - size;
		pos2[i] = pos[i] + size;
	}

	{
		boost::unique_lock<boost::mutex> lock(tree_mutex);
		return AcquireZoneAreaE( pos1, pos2, lock, flags );
	}
}

//-------------------------------------------------------------------------------------------------
void CellTree::ReleaseSE( boost::uint64_t shuffled ) {
	Leaf *leaf;
	int slot;
	GetActiveLeafSlot( shuffled, &leaf, &slot );

	leaf->locks[slot]--;
	leaf->total_cell_shares--;
	leaf->total_references--;
	GarbageTest( leaf );
	
	if( leaf->locks[slot] == 0 ) {
		cvar_lock_changes.notify_all();
	}
}

//-------------------------------------------------------------------------------------------------
void CellTree::ReleaseS( boost::uint64_t shuffled ) {
	boost::lock_guard<boost::mutex> lock(tree_mutex);
	ReleaseSE( shuffled );
}

//-------------------------------------------------------------------------------------------------
void CellTree::Release( int x, int y, int z ) {
	boost::lock_guard<boost::mutex> lock(tree_mutex);
	ReleaseSE( ShuffleCoords( x, y, z ) );
}

//-------------------------------------------------------------------------------------------------
void CellTree::Release( boost::uint64_t index ) {
	boost::lock_guard<boost::mutex> lock(tree_mutex);
	ReleaseSE( ShuffleCoords( index ) );
}

//-------------------------------------------------------------------------------------------------
void CellTree::ReleaseAreaE( const int *pos1, const int *pos2 ) {

	int cpos1[3],cpos2[3];
	for( int i = 0; i < 3; i++ ) {
		cpos1[i] = pos1[i] >> 4;
		cpos2[i] = pos2[i] >> 4;
	}

	for( int y = cpos1[1]; y <= cpos2[1]; y++ ) {
		for( int z = cpos1[2]; z <= cpos2[2]; z++ ) {
			for( int x = cpos1[0]; x <= cpos2[0]; x++ ) {
				ReleaseSE( ShuffleCoords(x, y, z) );
			}
		}
	}
}

//-------------------------------------------------------------------------------------------------
void CellTree::ReleaseArea( int x1, int y1, int z1, int x2, int y2, int z2 ) {
	int pos1[3], pos2[3];
	SetupArea( pos1, pos2, x1, y1, z1, x2, y2, z2 );
	ReleaseArea( pos1, pos2 );
}

//-------------------------------------------------------------------------------------------------
void CellTree::ReleaseArea( int x, int y, int z, int size ) {
	int pos1[3], pos2[3];
	SetupArea( pos1, pos2, x, y, z, size );
	ReleaseArea( pos1, pos2 );
}

//-------------------------------------------------------------------------------------------------
void CellTree::ReleaseArea( const int *pos1, const int *pos2 ) {
	boost::lock_guard<boost::mutex> lock(tree_mutex);
	ReleaseAreaE( pos1, pos2 );
}

//-------------------------------------------------------------------------------------------------
void CellTree::ReleaseArea( const int *pos, int size ) {
	int pos1[3], pos2[3];
	SetupArea( pos1, pos2, pos, size );
	ReleaseArea( pos1, pos2 );
}

//-------------------------------------------------------------------------------------------------
void CellTree::ReleaseZoneSE( boost::uint64_t shuffled ) {
	Leaf *leaf;
	GetActiveLeafSlot( shuffled, &leaf, 0 );
	leaf->zone_shares--;
	leaf->total_references--;
	GarbageTest( leaf );

	if( leaf->zone_shares == 0 ) {
		cvar_lock_changes.notify_all();
	}
}

//-------------------------------------------------------------------------------------------------
void CellTree::ReleaseZoneS( boost::uint64_t shuffled ) {
	boost::lock_guard<boost::mutex> lock(tree_mutex);
	ReleaseZoneSE( shuffled );
}

//-------------------------------------------------------------------------------------------------
void CellTree::ReleaseZone( int x, int y, int z ) {
	ReleaseZoneS( ShuffleCoords( x, y, z ) );
}

//-------------------------------------------------------------------------------------------------
void CellTree::ReleaseZone( boost::uint64_t index ) {
	ReleaseZoneS( ShuffleCoords( index ) );
}

//-------------------------------------------------------------------------------------------------
void CellTree::ReleaseZoneAreaE( const int *pos1, const int *pos2 ) {
	int cpos1[3], cpos2[3];
	cpos1[0] = pos1[0] >> 8;
	cpos1[1] = pos1[1] >> 8;
	cpos1[2] = pos1[2] >> 7;

	cpos2[0] = pos2[0] >> 8;
	cpos2[1] = pos2[1] >> 8;
	cpos2[2] = pos2[2] >> 7;

	for( int y = cpos1[1]; y <= cpos2[1]; y++ ) {
		for( int z = cpos1[2]; z <= cpos2[2]; z++ ) {
			for( int x = cpos1[0]; x <= cpos2[0]; x++ ) {
				ReleaseZoneSE( ShuffleCoords(x, y, z) );
			}
		}
	}
}

//-------------------------------------------------------------------------------------------------
void CellTree::ReleaseZoneArea( int x1, int y1, int z1, int x2, int y2, int z2 ) {
	int pos1[3],pos2[3];
	SetupArea( pos1, pos2, x1, y1, z1, x2, y2, z2 );
	ReleaseZoneArea( pos1, pos2 );
}

//-------------------------------------------------------------------------------------------------
void CellTree::ReleaseZoneArea( int x, int y, int z, int size ) {
	int pos1[3], pos2[3];
	SetupArea( pos1, pos2, x, y, z, size );
}

//-------------------------------------------------------------------------------------------------
void CellTree::ReleaseZoneArea( const int *pos1, const int *pos2 ) {
	boost::lock_guard<boost::mutex> lock(tree_mutex);
	ReleaseZoneAreaE( pos1, pos2 );
}

//-------------------------------------------------------------------------------------------------
void CellTree::ReleaseZoneArea( const int *pos, int size ) {
	int pos1[3], pos2[3];
	SetupArea( pos1, pos2, pos, size );
	ReleaseZoneArea( pos1, pos2 );
}

//-------------------------------------------------------------------------------------------------
int CellTree::ReserveSE( boost::uint64_t shuffled, boost::unique_lock<boost::mutex> &lock, int flags ) {
	int result;
	while( true ) {
		int slot;
		Leaf *leaf;
		if( flags & FLAG_KNOWNONLY ) {
			bool exists = GetLeafSlot( shuffled, &leaf, &slot );
			if( !exists ) {
				return RESULT_NOTKNOWN;
			}
		} else {
			GetLeafSlotOrCreate( shuffled, &leaf, &slot );
		}
		if( leaf->zone_xlock != LEAF_XLOCK_OFF ) {
			if( flags & FLAG_NOWAIT ) {
				return RESULT_EXLOCKED;
			} else {
				cvar_lock_changes.wait( lock );
			}
		}
		int state = leaf->state[slot];

		if( (!(state&CELLSTATE_READY)) && (flags & FLAG_KNOWNONLY) ) {
			return RESULT_NOTKNOWN;
		}

		if( state & CELLSTATE_RESERVED ) {
			if( flags & FLAG_NOWAIT ) {
				return RESULT_EXLOCKED;
			} else {
				cvar_lock_changes.wait( lock );
			}
		}

		if( flags & FLAG_TEST ) {
			return RESULT_SUCCESS;
		}

		leaf->state[slot] |= CELLSTATE_RESERVED;
		leaf->total_cell_reserves++;
		leaf->total_references++;
		
		return RESULT_SUCCESS;
	}
}

//-------------------------------------------------------------------------------------------------
int CellTree::ReserveS( boost::uint64_t shuffled, int flags ) {
	boost::unique_lock<boost::mutex> lock(tree_mutex);
	return ReserveSE( shuffled, lock, flags );
}

//-------------------------------------------------------------------------------------------------
int CellTree::Reserve( int x, int y, int z, int flags ) {
	return ReserveS( ShuffleCoords( x, y, z ), flags );
}

//-------------------------------------------------------------------------------------------------
int CellTree::Reserve( boost::uint64_t index, int flags ) {
	return ReserveS( ShuffleCoords( index ), flags );
}

//-------------------------------------------------------------------------------------------------
int CellTree::ReserveAreaE( const int *pos1, const int *pos2, boost::unique_lock<boost::mutex> &lock, int flags ) {
	int cpos1[3],cpos2[3];
	for( int i = 0; i < 3; i++ ) {
		cpos1[i] = pos1[i] >> 4;
		cpos2[i] = pos2[i] >> 4;
	}

	while( true ) {
		int test_result = RESULT_INVALIDAREA;
		for( int y = cpos1[1]; y <= cpos2[1]; y++ ) {
			for( int z = cpos1[2]; z <= cpos2[2]; z++ ) {
				for( int x = cpos1[0]; x <= cpos2[0]; x++ ) {
					test_result = ReserveSE( ShuffleCoords(x, y, z), lock, flags|FLAG_TEST );

					if( test_result != RESULT_SUCCESS )
						goto breakloop;
				}
			}
		}
		breakloop:
		if( test_result != RESULT_SUCCESS ) {
			if( test_result == RESULT_INVALIDAREA || test_result == RESULT_NOTKNOWN ) {
				return test_result;
			}
			if( (flags & FLAG_NOWAIT) && (test_result == RESULT_EXLOCKED || test_result == RESULT_TOOMANY) ) {
				return test_result;
			}

			if( !(flags & FLAG_TEST ) ) {
				cvar_lock_changes.wait( lock );
				continue;
			} else {
				return test_result;
			}
		}

		if( flags & FLAG_TEST ) {
			return RESULT_SUCCESS;
		}

		for( int y = cpos1[1]; y <= cpos2[1]; y++ ) {
			for( int z = cpos1[2]; z <= cpos2[2]; z++ ) {
				for( int x = cpos1[0]; x <= cpos2[0]; x++ ) {
					ReserveSE( ShuffleCoords(x, y, z), lock, flags );
				}
			}
		}
		
		return RESULT_SUCCESS;
	}
}

//-------------------------------------------------------------------------------------------------
int CellTree::ReserveArea( int x1, int y1, int z1, int x2, int y2, int z2, int flags ) {
	int pos1[3], pos2[3];
	SetupArea( pos1, pos2, x1, y1, z1, x2, y2, z2 );
	return ReserveArea( pos1, pos2, flags );
}

//-------------------------------------------------------------------------------------------------
int CellTree::ReserveArea( int x, int y, int z, int size, int flags ) {
	int pos1[3], pos2[3];
	SetupArea( pos1, pos2, x, y, z, size );
	return ReserveArea( pos1, pos2, flags );
}

//-------------------------------------------------------------------------------------------------
int CellTree::ReserveArea( const int *pos1, const int *pos2, int flags ) {
	boost::unique_lock<boost::mutex> lock(tree_mutex);
	return ReserveAreaE( pos1, pos2, lock, flags );
}

//-------------------------------------------------------------------------------------------------
int CellTree::ReserveArea( const int *pos, int size, int flags ) {
	int pos1[3], pos2[3];
	SetupArea( pos1, pos2, pos, size );
	return ReserveArea( pos1, pos2, flags );
}

//-------------------------------------------------------------------------------------------------
int CellTree::ReserveZoneSE( boost::uint64_t shuffled, boost::unique_lock<boost::mutex> &lock, int flags ) {
	int result;
	while( true ) {
		Leaf *leaf;
		GetLeafSlotOrCreate( shuffled, &leaf, 0 );

		if( leaf->zone_xlock != 0 || leaf->total_cell_reserves != 0 ) {
			if( flags & FLAG_NOWAIT ) {
				return RESULT_EXLOCKED;
			} else {
				cvar_lock_changes.wait(lock);
				continue;
			}
		}


		
		if( flags & FLAG_TEST ) {
			return RESULT_SUCCESS;
		}

		leaf->zone_xlock = LEAF_XLOCK_RESERVED;
		leaf->total_references++;
		return RESULT_SUCCESS;
	}
}

//-------------------------------------------------------------------------------------------------
int CellTree::ReserveZoneS( boost::uint64_t shuffled, int flags ) {
	boost::unique_lock<boost::mutex> lock(tree_mutex);
	return ReserveZoneSE( shuffled, lock, flags );
}

//-------------------------------------------------------------------------------------------------
int CellTree::ReserveZone( int x, int y, int z, int flags ) {
	return ReserveZoneS( ShuffleCoords( x, y, z ), flags );
}

//-------------------------------------------------------------------------------------------------
int CellTree::ReserveZone( boost::uint64_t index, int flags ) {
	return ReserveZoneS( ShuffleCoords( index ), flags );
}

//-------------------------------------------------------------------------------------------------
int CellTree::ReserveZoneAreaE( const int *pos1, const int *pos2, boost::unique_lock<boost::mutex> &lock, int flags ) {
	int cpos1[3], cpos2[3];
	cpos1[0] = pos1[0] >> 8;
	cpos1[1] = pos1[1] >> 8;
	cpos1[2] = pos1[2] >> 7;

	cpos2[0] = pos2[0] >> 8;
	cpos2[1] = pos2[1] >> 8;
	cpos2[2] = pos2[2] >> 7;

	while( true ) {
		int test_result = RESULT_INVALIDAREA;
		for( int y = cpos1[1]; y <= cpos2[1]; y++ ) {
			for( int z = cpos1[2]; z <= cpos2[2]; z++ ) {
				for( int x = cpos1[0]; x <= cpos2[0]; x++ ) {
					test_result = ReserveZoneSE( ShuffleCoords(x, y, z), lock, flags|FLAG_TEST );

					if( test_result != RESULT_SUCCESS )
						goto breakloop;
				}
			}
		}
		breakloop:
		if( test_result != RESULT_SUCCESS ) {
			if( test_result == RESULT_INVALIDAREA ) {
				return test_result;
			}
			if( (flags & FLAG_NOWAIT) && (test_result == RESULT_EXLOCKED) ) {
				return test_result;
			}

			if( !(flags & FLAG_TEST) ) {
				cvar_lock_changes.wait(lock);
				continue;
			} else {
				return test_result;
			}
		}

		if( flags & FLAG_TEST ) {
			return RESULT_SUCCESS;
		}

		for( int y = cpos1[1]; y <= cpos2[1]; y++ ) {
			for( int z = cpos1[2]; z <= cpos2[2]; z++ ) {
				for( int x = cpos1[0]; x <= cpos2[0]; x++ ) {
					ReserveZoneSE( ShuffleCoords(x, y, z), lock, flags );
				}
			}
		} 
		return RESULT_SUCCESS; 
	}
}

//-------------------------------------------------------------------------------------------------
int CellTree::ReserveZoneArea( int x1, int y1, int z1, int x2, int y2, int z2, int flags ) {
	int pos1[3], pos2[3];
	SetupArea( pos1, pos2, x1, y1, z1, x2, y2, z2 );

	return ReserveZoneArea( pos1, pos2, flags );
}

//-------------------------------------------------------------------------------------------------
int CellTree::ReserveZoneArea( int x, int y, int z, int size, int flags ) {
	int pos1[3], pos2[3];
	SetupArea( pos1, pos2, x, y, z, size );
	return ReserveZoneArea( pos1, pos2, flags );
}

//-------------------------------------------------------------------------------------------------
int CellTree::ReserveZoneArea( const int *pos1, const int *pos2, int flags ) {
	boost::unique_lock<boost::mutex> lock(tree_mutex);
	return ReserveZoneAreaE( pos1, pos2, lock, flags );
}

//-------------------------------------------------------------------------------------------------
int CellTree::ReserveZoneArea( const int *pos, const int size, int flags ) {
	int pos1[3], pos2[3];
	return ReserveZoneArea( pos1, pos2, flags );
}

//-------------------------------------------------------------------------------------------------
int CellTree::UpgradeSE( boost::uint64_t shuffled, boost::unique_lock<boost::mutex> &lock, int flags ) {
	int result;

	int slot;
	Leaf *leaf;

	GetActiveLeafSlot( shuffled, &leaf, &slot );

	if( !(leaf->state[slot] & CELLSTATE_LOCKED) ) {
		leaf->state[slot] |= CELLSTATE_LOCKED;
		leaf->total_cell_xlocks++;
	}

	while( true ) {
		
		
		if( leaf->locks[slot] != 0 || leaf->zone_shares != 0 ) {
			if( flags & FLAG_NOWAIT ) {
				return RESULT_SHARED_ACTIVE;
			} else {
				cvar_lock_changes.wait(lock);
				continue;
			}
		}
		return RESULT_SUCCESS;
	}
}

//-------------------------------------------------------------------------------------------------
int CellTree::UpgradeS( boost::uint64_t shuffled, int flags ) {
	boost::unique_lock<boost::mutex> lock(tree_mutex);
	return UpgradeSE( shuffled, lock, flags );
}

//-------------------------------------------------------------------------------------------------
int CellTree::Upgrade( int x, int y, int z, int flags ) {
	return UpgradeS( ShuffleCoords( x, y, z ), flags );
}

//-------------------------------------------------------------------------------------------------
int CellTree::Upgrade( boost::uint64_t index, int flags ) {
	return UpgradeS( ShuffleCoords( index ), flags );
}

//-------------------------------------------------------------------------------------------------
int CellTree::UpgradeAreaE( const int *pos1, const int *pos2, boost::unique_lock<boost::mutex> &lock, int flags ) {

	int cpos1[3],cpos2[3];
	for( int i = 0; i < 3; i++ ) {
		cpos1[i] = pos1[i] >> 4;
		cpos2[i] = pos2[i] >> 4;
	}

	if( !(flags & FLAG_ALREADYLOCKED ) ) {
		for( int y = cpos1[1]; y <= cpos2[1]; y++ ) {
			for( int z = cpos1[2]; z <= cpos2[2]; z++ ) {
				for( int x = cpos1[0]; x <= cpos2[0]; x++ ) {
					
					Leaf *leaf;
					int slot;
					GetActiveLeafSlot( ShuffleCoords( x, y, z ), &leaf, &slot );
					
					if( !(leaf->state[slot] & CELLSTATE_LOCKED) ) {
						leaf->state[slot] |= CELLSTATE_LOCKED;
						leaf->total_cell_xlocks++;
					}
				}
			}
		} 
	}

	while( true ) {

		bool complete = true;

		for( int y = cpos1[1]; y <= cpos2[1]; y++ ) {
			for( int z = cpos1[2]; z <= cpos2[2]; z++ ) {
				for( int x = cpos1[0]; x <= cpos2[0]; x++ ) {
					
					Leaf *leaf;
					int slot;
					GetActiveLeafSlot( ShuffleCoords( x, y, z ), &leaf, &slot );
					
					if( leaf->locks[slot] != 0 || leaf->zone_shares != 0 ) {
						complete = false;
						goto breakloop;
					}
					
				}
			}
		}
		breakloop:
		if( !complete ) {
			if( flags & FLAG_NOWAIT ) {
				return RESULT_SHARED_ACTIVE;
			} else {
				cvar_lock_changes.wait(lock);
				continue;
			}
		}
		
		return RESULT_SUCCESS;
	}
}

//-------------------------------------------------------------------------------------------------
int CellTree::UpgradeArea( int x1, int y1, int z1, int x2, int y2, int z2, int flags ) {
	int pos1[3], pos2[3];
	SetupArea( pos1, pos2, x1, y1, z1, x2, y2, z2 );
	return UpgradeArea( pos1, pos2, flags );
}

//-------------------------------------------------------------------------------------------------
int CellTree::UpgradeArea( int x, int y, int z, int size, int flags ) {
	int pos1[3], pos2[3];
	SetupArea( pos1, pos2, x, y, z, size );
	return UpgradeArea( pos1, pos2, flags );
}

//-------------------------------------------------------------------------------------------------
int CellTree::UpgradeArea( const int *pos1, const int *pos2, int flags ) {
	boost::unique_lock<boost::mutex> lock(tree_mutex);
	return UpgradeAreaE( pos1, pos2, lock, flags );
}

//-------------------------------------------------------------------------------------------------
int CellTree::UpgradeArea( const int *pos, int size, int flags ) {
	int pos1[3], pos2[3];
	SetupArea( pos1, pos2, pos, size );
	return UpgradeArea( pos1, pos2, flags );
}

//-------------------------------------------------------------------------------------------------
int CellTree::UpgradeZoneSE( boost::uint64_t shuffled, boost::unique_lock<boost::mutex> &lock, int flags ) {

	Leaf *leaf;
	GetActiveLeafSlot( shuffled, &leaf, 0 );
	if( leaf->zone_xlock != LEAF_XLOCK_ACTIVE ) {
		leaf->zone_xlock = LEAF_XLOCK_ACTIVE;
		// todo something here?
	}

	while( true ) {
		if( leaf->total_cell_shares != 0 || leaf->zone_shares != 0 ) {
			if( flags & FLAG_NOWAIT ) {
				return RESULT_SHARED_ACTIVE;
			} else {
				cvar_lock_changes.wait(lock);
				continue;
			}
		}
		return RESULT_SUCCESS;
	}
}

//-------------------------------------------------------------------------------------------------
int CellTree::UpgradeZoneS( boost::uint64_t shuffled, int flags ) {
	boost::unique_lock<boost::mutex> lock(tree_mutex);
	return UpgradeZoneSE( shuffled, lock, flags );
}

//-------------------------------------------------------------------------------------------------
int CellTree::UpgradeZone( int x, int y, int z, int flags ) {
	return UpgradeZoneS( ShuffleCoords( x, y, z ), flags );
}

//-------------------------------------------------------------------------------------------------
int CellTree::UpgradeZone( boost::uint64_t index, int flags ) {
	return UpgradeZoneS( ShuffleCoords( index ), flags );
}

//-------------------------------------------------------------------------------------------------
int CellTree::UpgradeZoneAreaE( const int *pos1, const int *pos2, boost::unique_lock<boost::mutex> &lock, int flags ) {
	int cpos1[3],cpos2[3];
	cpos1[0] = pos1[0] >> 8;
	cpos1[1] = pos1[1] >> 8;
	cpos1[2] = pos1[2] >> 7;

	cpos2[0] = pos2[0] >> 8;
	cpos2[1] = pos2[1] >> 8;
	cpos2[2] = pos2[2] >> 7;

	if( !(flags & FLAG_ALREADYLOCKED ) ) {
		for( int y = cpos1[1]; y <= cpos2[1]; y++ ) {
			for( int z = cpos1[2]; z <= cpos2[2]; z++ ) {
				for( int x = cpos1[0]; x <= cpos2[0]; x++ ) {
					
					Leaf *leaf;
					GetActiveLeafSlot( ShuffleCoords( x, y, z ), &leaf, 0 );
					leaf->zone_xlock = LEAF_XLOCK_ACTIVE;
				}
			}
		} 
	}

	while( true ) {
		bool complete = true;

		for( int y = cpos1[1]; y <= cpos2[1]; y++ ) {
			for( int z = cpos1[2]; z <= cpos2[2]; z++ ) {
				for( int x = cpos1[0]; x <= cpos2[0]; x++ ) {
					
					Leaf *leaf;
					GetActiveLeafSlot( ShuffleCoords( x, y, z ), &leaf, 0 );
					
					if( leaf->total_cell_shares != 0 || leaf->zone_shares != 0 ) {
						complete = false;
						goto breakloop;
					}
					
				}
			}
		} 
		breakloop:
		if( !complete ) {
			if( flags & FLAG_NOWAIT ) {
				return RESULT_SHARED_ACTIVE;
			} else {
				cvar_lock_changes.wait( lock );
				continue;
			}
		}

		return RESULT_SUCCESS;
	}
}

//-------------------------------------------------------------------------------------------------
int CellTree::UpgradeZoneArea( int x1, int y1, int z1, int x2, int y2, int z2, int flags ) {
	int pos1[3], pos2[3];
	SetupArea( pos1, pos2, x1, y1, z1, x2, y2, z2 );
	return UpgradeZoneArea( pos1, pos2, flags );
}

//-------------------------------------------------------------------------------------------------
int CellTree::UpgradeZoneArea( int x, int y, int z, int size, int flags ) {
	int pos1[3], pos2[3];
	SetupArea( pos1, pos2, x, y, z, size );
	return UpgradeZoneArea( pos1, pos2, flags );
}

//-------------------------------------------------------------------------------------------------
int CellTree::UpgradeZoneArea( const int *pos1, const int *pos2, int flags ) {
	boost::unique_lock<boost::mutex> lock(tree_mutex);
	return UpgradeZoneAreaE( pos1, pos2, lock, flags );
}

//-------------------------------------------------------------------------------------------------
int CellTree::UpgradeZoneArea( const int *pos, int size, int flags ) {
	int pos1[3], pos2[3];
	SetupArea( pos1, pos2, pos, size );
	return UpgradeZoneArea( pos1, pos2, flags );
}

//-------------------------------------------------------------------------------------------------
void CellTree::CompleteSE( boost::uint64_t shuffled, int flags ) {
	Leaf *leaf;
	int slot;
	GetActiveLeafSlot( shuffled, &leaf, &slot );
	if( leaf->state[slot] & CELLSTATE_LOCKED ) {
		leaf->total_cell_xlocks--;
	}
	leaf->state[slot] &= ~(CELLSTATE_RESERVED|CELLSTATE_LOCKED);
	leaf->total_cell_reserves--;
	leaf->total_references--;
	GarbageTest( leaf );
	
	cvar_lock_changes.notify_all();
}

//-------------------------------------------------------------------------------------------------
void CellTree::CompleteS( boost::uint64_t shuffled, int flags ) {
	boost::lock_guard<boost::mutex> lock(tree_mutex);
	CompleteSE( shuffled, flags );
}

//-------------------------------------------------------------------------------------------------
void CellTree::Complete( int x, int y, int z, int flags ) {
	CompleteS( ShuffleCoords( x, y, z ), flags );
}

//-------------------------------------------------------------------------------------------------
void CellTree::Complete( boost::uint64_t index, int flags ) {
	CompleteS( ShuffleCoords( index ), flags );
}

//-------------------------------------------------------------------------------------------------
void CellTree::CompleteAreaE( const int *pos1, const int *pos2, int flags ) {
	int cpos1[3],cpos2[3];
	for( int i = 0; i < 3; i++ ) {
		cpos1[i] = pos1[i] >> 4;
		cpos2[i] = pos2[i] >> 4;
	}

	for( int y = cpos1[1]; y <= cpos2[1]; y++ ) {
		for( int z = cpos1[2]; z <= cpos2[2]; z++ ) {
			for( int x = cpos1[0]; x <= cpos2[0]; x++ ) {
				CompleteSE( ShuffleCoords(x, y, z) );
			}
		}
	}
}

//-------------------------------------------------------------------------------------------------
void CellTree::CompleteArea( int x1, int y1, int z1, int x2, int y2, int z2, int flags ) {
	int pos1[3], pos2[3];
	SetupArea( pos1, pos2, x1, y1, z1, x2, y2, z2 );
	CompleteArea( pos1, pos2, flags );
}

//-------------------------------------------------------------------------------------------------
void CellTree::CompleteArea( int x, int y, int z, int size, int flags ) {
	int pos1[3], pos2[3];
	SetupArea( pos1, pos2, x, y, z, size );
	CompleteArea( pos1, pos2, flags );
}

//-------------------------------------------------------------------------------------------------
void CellTree::CompleteArea( const int *pos1, const int *pos2, int flags ) {
	boost::lock_guard<boost::mutex> lock(tree_mutex);
	CompleteAreaE( pos1, pos2, flags );
}

//-------------------------------------------------------------------------------------------------
void CellTree::CompleteArea( const int *pos, int size, int flags ) {
	int pos1[3], pos2[3];
	SetupArea( pos1, pos2, pos, size );
	CompleteArea( pos1, pos2, flags );
}

//-------------------------------------------------------------------------------------------------
void CellTree::CompleteZoneSE( boost::uint64_t shuffled, int flags ) {
	Leaf *leaf;
	GetActiveLeafSlot( shuffled, &leaf, 0 );
	if( leaf->zone_xlock == LEAF_XLOCK_ACTIVE ) {
		// um?
	}
	leaf->zone_xlock = LEAF_XLOCK_OFF;
	leaf->total_references--;
	GarbageTest( leaf );
	cvar_lock_changes.notify_all();
}

//-------------------------------------------------------------------------------------------------
void CellTree::CompleteZoneS( boost::uint64_t shuffled, int flags ) {
	boost::lock_guard<boost::mutex> lock(tree_mutex);
	CompleteZoneSE( shuffled, flags );
}

//-------------------------------------------------------------------------------------------------
void CellTree::CompleteZone( int x, int y, int z, int flags ) {
	CompleteZoneS( ShuffleCoords( x, y, z ), flags );
}

//-------------------------------------------------------------------------------------------------
void CellTree::CompleteZone( boost::uint64_t index, int flags ) {
	CompleteZoneS( ShuffleCoords( index ), flags );
}

//-------------------------------------------------------------------------------------------------
void CellTree::CompleteZoneAreaE( const int *pos1, const int *pos2, int flags ) {
	int cpos1[3], cpos2[3];
	cpos1[0] = pos1[0] >> 8;
	cpos1[1] = pos1[1] >> 8;
	cpos1[2] = pos1[2] >> 7;

	cpos2[0] = pos2[0] >> 8;
	cpos2[1] = pos2[1] >> 8;
	cpos2[2] = pos2[2] >> 7;

	for( int y = cpos1[1]; y <= cpos2[1]; y++ ) {
		for( int z = cpos1[2]; z <= cpos2[2]; z++ ) {
			for( int x = cpos1[0]; x <= cpos2[0]; x++ ) {
				CompleteZoneSE( ShuffleCoords(x, y, z), flags );
			}
		}
	}
}

//-------------------------------------------------------------------------------------------------
void CellTree::CompleteZoneArea( int x1, int y1, int z1, int x2, int y2, int z2, int flags ) {
	int pos1[3], pos2[3];
	SetupArea( pos1, pos2, x1, y1, z1, x2, y2, z2 );
	CompleteZoneArea( pos1, pos2, flags );
}

//-------------------------------------------------------------------------------------------------
void CellTree::CompleteZoneArea( int x, int y, int z, int size, int flags ) {
	int pos1[3], pos2[3];
	SetupArea( pos1, pos2, x, y, z, size );
	CompleteZoneArea( pos1, pos2, flags );
}

//-------------------------------------------------------------------------------------------------
void CellTree::CompleteZoneArea( const int *pos1, const int *pos2, int flags ) {
	boost::lock_guard<boost::mutex> lock(tree_mutex);
	CompleteZoneAreaE( pos1, pos2, flags );
}

//-------------------------------------------------------------------------------------------------
void CellTree::CompleteZoneArea( const int *pos, int size, int flags ) {
	int pos1[3], pos2[3];
	CompleteZoneArea( pos1, pos2, flags );
}

//-------------------------------------------------------------------------------------------------
int CellTree::WaitCreationSE( boost::uint64_t shuffled, boost::unique_lock<boost::mutex> &lock ) {
	Leaf *leaf;
	int slot;
	if( !GetLeafSlot( shuffled, &leaf, &slot ) ) {
		return RESULT_NO_CREATION;
	}

	while( true ) {
		if( leaf->state[slot] & CELLSTATE_READY ) {
			return RESULT_SUCCESS;
		}

		if( leaf->state[slot] & CELLSTATE_CREATION ) {
			cvar_creation.wait( lock );
			continue;
		}
		return RESULT_NO_CREATION;
	}
}

//-------------------------------------------------------------------------------------------------
int CellTree::WaitCreationS( boost::uint64_t shuffled ) {
	boost::unique_lock<boost::mutex> lock(tree_mutex);
	return WaitCreationSE( shuffled, lock );
}

//-------------------------------------------------------------------------------------------------
int CellTree::WaitCreation( int x, int y, int z ) {
	return WaitCreationS( ShuffleCoords( x, y, z ) );
}

//-------------------------------------------------------------------------------------------------
int CellTree::WaitCreation( boost::uint64_t index ) {
	return WaitCreationS( ShuffleCoords( index ) );
}

//-------------------------------------------------------------------------------------------------
int CellTree::WaitCreationAreaE( const int *pos1, const int *pos2, boost::unique_lock<boost::mutex> &lock ) {
	int cpos1[3],cpos2[3];
	for( int i = 0; i < 3; i++ ) {
		cpos1[i] = pos1[i] >> 4;
		cpos2[i] = pos2[i] >> 4;
	}

	int test_result = RESULT_INVALIDAREA;
	for( int y = cpos1[1]; y <= cpos2[1]; y++ ) {
		for( int z = cpos1[2]; z <= cpos2[2]; z++ ) {
			for( int x = cpos1[0]; x <= cpos2[0]; x++ ) {
				test_result = WaitCreationSE( ShuffleCoords(x, y, z), lock );

				if( test_result != RESULT_SUCCESS )
					return test_result;
			}
		}
	}

	return test_result;
}

//-------------------------------------------------------------------------------------------------
int CellTree::WaitCreationArea( int x1, int y1, int z1, int x2, int y2, int z2 ) {
	int pos1[3], pos2[3];
	SetupArea( pos1, pos2, x1, y1, z1, x2, y2, z2 );
	return WaitCreationArea( pos1, pos2 );
}

//-------------------------------------------------------------------------------------------------
int CellTree::WaitCreationArea( int x, int y, int z, int size ) {
	int pos1[3], pos2[3];
	SetupArea( pos1, pos2, x, y, z, size );
	return WaitCreationArea( pos1, pos2 );
}

//-------------------------------------------------------------------------------------------------
int CellTree::WaitCreationArea( const int *pos1, const int *pos2 ) {
	boost::unique_lock<boost::mutex> lock(tree_mutex);
	return WaitCreationAreaE( pos1, pos2, lock );
}

//-------------------------------------------------------------------------------------------------
int CellTree::WaitCreationArea( const int *pos, int size ) {
	int pos1[3], pos2[3];
	SetupArea( pos1, pos2, pos, size );
	return WaitCreationArea( pos1, pos2 );
}

//-------------------------------------------------------------------------------------------------
bool CellTree::IsCreatedSE( boost::uint64_t shuffled ) {
	Leaf *leaf;
	int slot;
	if( !GetLeafSlot( shuffled, &leaf, &slot ) ) return false;
	
	if( leaf->state[slot] & CELLSTATE_READY ) return true;
	return false;
}

//-------------------------------------------------------------------------------------------------
bool CellTree::IsCreatedS( boost::uint64_t shuffled ) {
	boost::lock_guard<boost::mutex> lock(tree_mutex);
	return IsCreatedSE( shuffled );
}

//-------------------------------------------------------------------------------------------------
bool CellTree::IsCreated( int x, int y, int z ) {
	return IsCreatedS( ShuffleCoords( x, y, z ) );
}

//-------------------------------------------------------------------------------------------------
bool CellTree::IsCreated( boost::uint64_t index ) {
	return IsCreatedS( ShuffleCoords( index ) );
}

//-------------------------------------------------------------------------------------------------
bool CellTree::IsAreaCreatedE( const int *pos1, const int *pos2 ) {
	int cpos1[3],cpos2[3];
	for( int i = 0; i < 3; i++ ) {
		cpos1[i] = pos1[i] >> 4;
		cpos2[i] = pos2[i] >> 4;
	}

	for( int y = cpos1[1]; y <= cpos2[1]; y++ ) {
		for( int z = cpos1[2]; z <= cpos2[2]; z++ ) {
			for( int x = cpos1[0]; x <= cpos2[0]; x++ ) {
				if( !IsCreatedSE( ShuffleCoords( x, y, z ) ) ) return false;
			}
		}
	}
	return true;
}

//-------------------------------------------------------------------------------------------------
bool CellTree::IsAreaCreated( int x1, int y1, int z1, int x2, int y2, int z2 ) {
	int pos1[3], pos2[3];
	SetupArea( pos1, pos2, x1, y1, z1, x2, y2, z2 );
	return IsAreaCreated( pos1, pos2 );
}

//-------------------------------------------------------------------------------------------------
bool CellTree::IsAreaCreated( const int *pos1, const int *pos2 ) {
	boost::lock_guard<boost::mutex> lock(tree_mutex);
	return IsAreaCreatedE( pos1, pos2 );
}

//-------------------------------------------------------------------------------------------------
bool CellTree::IsAreaCreated( const int *pos, int size ) {
	int pos1[3], pos2[3];
	SetupArea( pos1, pos2, pos, size );
	return IsAreaCreated( pos1, pos2 );
}

//-------------------------------------------------------------------------------------------------
void CellTree::GetCellStateSE( boost::uint64_t shuffled, int *state, Cell **address ) {
	Leaf *leaf;
	int slot;
	if( !GetLeafSlot( shuffled, &leaf, &slot ) ) {
		if( state ) {
			*state = CELLSTATE_UNKNOWN;
		}
	}

	if( state ) *state = leaf->state[slot];
	if( address ) *address = leaf->data[slot];
}

//-------------------------------------------------------------------------------------------------
void CellTree::GetCellStateS( boost::uint64_t shuffled, int *state, Cell **address ) {
	boost::lock_guard<boost::mutex> lock(tree_mutex);
	GetCellStateSE( shuffled, state, address );
}

//-------------------------------------------------------------------------------------------------
void CellTree::GetCellState( int x, int y, int z, int *state, Cell **address ) {
	GetCellStateS( ShuffleCoords( x, y, z ), state, address );
}

//-------------------------------------------------------------------------------------------------
void CellTree::GetCellState( boost::uint64_t index, int *state, Cell **address ) {
	GetCellStateS( ShuffleCoords( index ), state, address );
}

//-------------------------------------------------------------------------------------------------
bool CellTree::IsLockedSE( boost::uint64_t shuffled ) {
	Leaf *leaf;
	int slot;
	if( !GetLeafSlot( shuffled, &leaf, &slot ) ) return false;

	if( leaf->locks[slot] ) return true;
	if( leaf->state[slot] & CELLSTATE_RESERVED ) return true;
	if( leaf->zone_shares ) return true;
	if( leaf->zone_xlock ) return true;
	return false;
}

//-------------------------------------------------------------------------------------------------
bool CellTree::IsLockedS( boost::uint64_t shuffled ) {
	boost::lock_guard<boost::mutex> lock(tree_mutex);
	return IsLockedSE( shuffled );
}

//-------------------------------------------------------------------------------------------------
bool CellTree::IsLocked( int x, int y, int z ) {
	return IsLockedS( ShuffleCoords( x, y, z ) );
}

//-------------------------------------------------------------------------------------------------
bool CellTree::IsLocked( boost::uint64_t index ) {
	return IsLockedS( ShuffleCoords( index ) );
}

//-------------------------------------------------------------------------------------------------
void CellTree::SetCellStateSE( boost::uint64_t shuffled, int state, Cell *address ) {
	Leaf *leaf;
	int slot;
	GetActiveLeafSlot( shuffled, &leaf, &slot );

	int old_state = leaf->state[slot];
	old_state = (old_state & CELLSTATE_READY) && !(old_state & CELLSTATE_EMPTY);
	int new_state = state;
	new_state = (old_state & CELLSTATE_READY) && !(old_state & CELLSTATE_EMPTY);
	if( old_state ) {
		if( !new_state ) leaf->total_references--;
		// note: dont do garbage check; this function is only called on locked data
	} else {
		if( new_state ) leaf->total_references++;
	}
	 
	leaf->state[slot] = state;
	leaf->data[slot] = address;
}

//-------------------------------------------------------------------------------------------------
void CellTree::SetCellStateS( boost::uint64_t shuffled, int state, Cell *address ) {
	boost::lock_guard<boost::mutex> lock(tree_mutex);
	SetCellStateSE( shuffled, state, address );
}

//-------------------------------------------------------------------------------------------------
void CellTree::SetCellState( int x, int y, int z, int state, Cell *address ) {
	SetCellStateS( ShuffleCoords( x, y, z ), state, address );
}

//-------------------------------------------------------------------------------------------------
void CellTree::SetCellState( boost::uint64_t index, int state, Cell *address ) {
	SetCellStateS( ShuffleCoords( index ), state, address );
}

//-------------------------------------------------------------------------------------------------
void CellTree::SetCellStateSE( boost::uint64_t shuffled, int state ) {
	Leaf *leaf;
	int slot;
	GetActiveLeafSlot( shuffled, &leaf, &slot );

	int old_state = leaf->state[slot];
	old_state = (old_state & CELLSTATE_READY) && !(old_state & CELLSTATE_EMPTY);
	int new_state = state;
	new_state = (old_state & CELLSTATE_READY) && !(old_state & CELLSTATE_EMPTY);
	if( old_state ) {
		if( !new_state ) leaf->total_references--;
		// note: dont do garbage check; this function is only called on locked data
	} else {
		if( new_state ) leaf->total_references++;
	}

	leaf->state[slot] = state;
}

//-------------------------------------------------------------------------------------------------
void CellTree::SetCellStateS( boost::uint64_t shuffled, int state ) {
	boost::lock_guard<boost::mutex> lock(tree_mutex);
	SetCellStateSE( shuffled, state );
}

//-------------------------------------------------------------------------------------------------
void CellTree::SetCellState( int x, int y, int z, int state ) {
	SetCellStateS( ShuffleCoords( x, y, z ), state );
}

//-------------------------------------------------------------------------------------------------
void CellTree::SetCellState( boost::uint64_t index, int state ) {
	SetCellStateS( ShuffleCoords( index ), state );
}

//-------------------------------------------------------------------------------------------------
void CellTree::SetUnknownCellStateSE( boost::uint64_t shuffled, int state ) {
	SetCellStateSE( shuffled, state, 0 );
}

//-------------------------------------------------------------------------------------------------
void CellTree::SetUnknownCellStateS( boost::uint64_t shuffled, int state ) {
	SetCellStateS( shuffled, state, 0 );
}

//-------------------------------------------------------------------------------------------------
void CellTree::SetUnknownCellState( int x, int y, int z, int state ) {
	SetCellState( x, y, z, state, 0 );
}

//-------------------------------------------------------------------------------------------------
void CellTree::SetUnknownCellState( boost::uint64_t index, int state ) {
	SetCellState( index, state, 0 );
}

//-------------------------------------------------------------------------------------------------
int CellTree::SampleE( int x, int y, int z ) {
	boost::uint64_t address = ShuffleCoords( x>>4, y>>4, z>>4 );
	Leaf *leaf;
	int slot;
	if( !GetLeafSlot( address, &leaf, &slot ) ) return -1;

	if( leaf->zone_xlock == LEAF_XLOCK_ACTIVE ) return -2;
	if( leaf->state[slot] & CELLSTATE_LOCKED ) return -2;

	if( !(leaf->state[slot] & CELLSTATE_READY) ) return -1;
	if( leaf->state[slot] & CELLSTATE_EMPTY ) return 0;

	int cubeslot = (x&15)+(y&15)*256+(z&15)*16;

	return leaf->data[slot]->cubes[cubeslot];
}

//-------------------------------------------------------------------------------------------------
int CellTree::Sample( int x, int y, int z ) {
	boost::unique_lock<boost::mutex> lock(tree_mutex);
	int result;
	while( true ) {
		int result = SampleE( x, y, z );
		if( result == -2 ) {
			cvar_lock_changes.wait( lock );
		} else {
			return result;
		}
	}
}

//-------------------------------------------------------------------------------------------------
}
