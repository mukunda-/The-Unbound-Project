//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//

#include "stdafx.h"

#if 0

namespace World {

//-------------------------------------------------------------------------------------------------
ChunkTree::Branch *ChunkTree::CreateBranch( Branch *parent, int slot ) {
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
ChunkTree::Leaf *ChunkTree::CreateLeaf( Branch *parent, int slot ) {
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
void ChunkTree::DeleteBranch( Branch *b ) {
	if( b == &trunk ) return;
	assert( b->counter == 0 );
	Branch *parent = b->parent;
	int slot = b->slot;
	Memory::FreeMem( b );
	if( parent ) {
		parent->data[slot] = 0;
		parent->counter--;
		if( parent->counter == 0 ) {
			DeleteBranch(parent);
		}
	}
}

//-------------------------------------------------------------------------------------------------
void ChunkTree::DeleteLeaf( Leaf *leaf ) {
	assert( leaf->counter == 0 );
	Branch *parent = leaf->parent;
	int slot = leaf->slot;
	Memory::FreeMem( leaf );
	if( parent ) {
		parent->data[slot] = 0;
		parent->counter--;
		if( parent->counter == 0 ) {
			DeleteBranch(parent);
		}
	}
}

//-------------------------------------------------------------------------------------------------
bool ChunkTree::GetLeafSlot( boost::uint64_t shuffled_coords, Leaf **r_leaf, int *r_slot ) {
	Branch *branch;

	branch = (Branch*)trunk.data[shuffled_coords>>44];
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
void ChunkTree::GetActiveLeafSlot( boost::uint64_t shuffled_coords, Leaf **r_leaf, int *r_slot ) {
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
	if( r_slot ) {
		*r_slot = addr;
	}
}

//-------------------------------------------------------------------------------------------------
ChunkTree::ChunkTree( int garbage_bytes_limit ) {

	inactive_leaf_first = 0;
	inactive_leaf_last = 0;
	
	memset( &trunk, 0, sizeof(trunk) );

	leaf_garbage_memory_total = 0;
	leaf_garbage_memory_limit = garbage_bytes_limit;
	leaf_garbage_memory_cleaned = garbage_bytes_limit * 90/100;
	leaf_cleanup_throttle = 11;
	InterlockedExchange( &leaf_cleanup_thread_active, 1 );
	

	leaf_cleanup_thread = boost::thread( boost::bind( &ChunkTree::LeafCleanupThread, this ) );
}

//-------------------------------------------------------------------------------------------------
ChunkTree::~ChunkTree() {

	leaf_cleanup_thread_active = 0;
	leaf_cleanup_thread.join();
}

//-------------------------------------------------------------------------------------------------
boost::uint64_t ChunkTree::PackCoords( int x, int y, int z ) {
	return x + (z<<16) + ((boost::uint64_t)y<<32);
}

//-------------------------------------------------------------------------------------------------
void ChunkTree::UnpackCoords( boost::uint64_t packed, int &x, int &y, int &z ) {
	
	x = packed & 0xFFFF;
	z = (packed >> 16) & 0xFFFF;
	y = (packed >> 32) & 0xFFFF;
}

//-------------------------------------------------------------------------------------------------
boost::uint64_t ChunkTree::ShuffleCoords( int x, int y, int z ) {
	return ((boost::uint64_t)(((x&15) + (z&15)*16 + (y&7)*16))) +
	       ((boost::uint64_t)(((x>>4)&15)+((z>>4)&15)*16+((y>>3)&7)*256)<<12) +
	       ((boost::uint64_t)(((x>>8)&15)+((z>>8)&15)*16+((y>>6)&7)*256)<<32) +
	       ((boost::uint64_t)(((x>>12)+(z>>12)*16+(y>>9)*256))<<44);
}

//-------------------------------------------------------------------------------------------------
int ChunkTree::TryAcquireInactiveS( boost::uint64_t shuffled_coords, int &original_state ) {
	boost::lock_guard<boost::mutex> lock(mut); // exclusive

	Branch *branch;

	branch = CreateBranch( &trunk, shuffled_coords>>44 );
	branch = CreateBranch( branch, (shuffled_coords>>32) & 2047 );
	Leaf *leaf = CreateLeaf( branch, (shuffled_coords>>12) & 2047 );

	int state = leaf->state[(shuffled_coords)&2047];
	if( state < CHUNKSTATE_READY ) {

		// lock chunk and return original state
		leaf->state[(shuffled_coords)&2047] = CHUNKSTATE_LOCKED;
		leaf->counter++;
		original_state = state;
		return ACQUIREINACTIVE_SUCCESS;
	} else {

		// chunk isn't inactive
		return ACQUIREINACTIVE_NOTINACTIVE;
	}

}
//-------------------------------------------------------------------------------------------------
int ChunkTree::TryAcquireInactive( int x, int y, int z, int &original_state ) {
	TryAcquireInactiveS( ShuffleCoords( x, y, z ), original_state );
}

//-------------------------------------------------------------------------------------------------
int ChunkTree::TryAcquireInactive( boost::uint64_t index, int &original_state ) {

	int x,y,z;
	UnpackCoords( index, x, y, z );

	return TryAcquireInactive( x, y, z, original_state );
}

//-------------------------------------------------------------------------------------------------
int ChunkTree::TryAcquireActiveES( boost::uint64_t shuffled_coords, Chunk **address ) {
	Branch *branch;
	if( !(branch = (Branch*)trunk.data[(shuffled_coords>>44)&2047]) ) return ACQUIREACTIVE_NOTACTIVE;
	if( !(branch = (Branch*)trunk.data[(shuffled_coords>>32)&2047]) ) return ACQUIREACTIVE_NOTACTIVE;
	Leaf *leaf;
	if( !(leaf = (Leaf*)trunk.data[(shuffled_coords>>12)&2047]) ) return ACQUIREACTIVE_NOTACTIVE;
	if( leaf->state[shuffled_coords&2047] < CHUNKSTATE_READY ) {
		if( leaf->state[shuffled_coords&2047] == CHUNKSTATE_EMPTY ) return ACQUIREACTIVE_NOTACTIVE_EMPTY;
		return ACQUIREACTIVE_NOTACTIVE;
	}

	if( leaf->state[shuffled_coords&2047] != CHUNKSTATE_READY ) {
		return ACQUIREACTIVE_ISLOCKED;
	}

	leaf->state[shuffled_coords&2047] = CHUNKSTATE_LOCKED;

	if( address ) {
		*address = leaf->data[shuffled_coords&2047];
	}
	return ACQUIREACTIVE_SUCCESS;
}

//-------------------------------------------------------------------------------------------------
int ChunkTree::TryAcquireActiveS( boost::uint64_t shuffled_coords, Chunk **address ) {
	boost::lock_guard<boost::mutex> lock(mut);

	return TryAcquireActiveES( shuffled_coords, address );
}

//-------------------------------------------------------------------------------------------------
int ChunkTree::TryAcquireActive( int x, int y, int z, Chunk **address ) {
	return TryAcquireActiveS( ShuffleCoords( x, y, z ), address );
}

//-------------------------------------------------------------------------------------------------
int ChunkTree::TryAcquireActive( boost::uint64_t index, Chunk **address ) {
	int x, y, z;
	UnpackCoords( index, x, y, z );
	return TryAcquireActive( x, y, z, address );
}

//-------------------------------------------------------------------------------------------------
int ChunkTree::WaitAcquireActiveS( boost::uint64_t shuffled_coords, Chunk **address ) {

	boost::unique_lock<boost::mutex> lock(mut);

	int result;

	while( true ) {

		result = TryAcquireActiveES( shuffled_coords, address );
		if( result == ACQUIREACTIVE_ISLOCKED ) {
			cvar_lockchanges.wait(lock);
		} else if( result == ACQUIREACTIVE_NOTACTIVE || result == ACQUIREACTIVE_NOTACTIVE_EMPTY ) {
			return result;
		} else if( result == ACQUIREACTIVE_SUCCESS ) {
			return result;
		}
	}
}

//-------------------------------------------------------------------------------------------------
int ChunkTree::WaitAcquireActive( int x, int y, int z, Chunk **address ) {
	return WaitAcquireActiveS( ShuffleCoords( x, y, z ), address );
}

//-------------------------------------------------------------------------------------------------
int ChunkTree::WaitAcquireActive( boost::uint64_t index, Chunk **address ) {
	int x, y, z;
	UnpackCoords( index, x, y, z );
	return WaitAcquireActive( x, y, z, address );
}

//-------------------------------------------------------------------------------------------------
int ChunkTree::TryAcquireActiveSharedES( boost::uint64_t shuffled_coords, Chunk **address ) {
	Branch *branch;
	if( !(branch = (Branch*)trunk.data[(shuffled_coords>>44)&2047]) ) return ACQUIREACTIVE_NOTACTIVE;
	if( !(branch = (Branch*)trunk.data[(shuffled_coords>>32)&2047]) ) return ACQUIREACTIVE_NOTACTIVE;
	Leaf *leaf;
	if( !(leaf = (Leaf*)trunk.data[(shuffled_coords>>12)&2047]) ) return ACQUIREACTIVE_NOTACTIVE;

	int slot = shuffled_coords&2047;

	if( leaf->state[slot] < CHUNKSTATE_READY ) {
		if( leaf->state[slot] == CHUNKSTATE_EMPTY ) return ACQUIREACTIVE_NOTACTIVE_EMPTY;
		return ACQUIREACTIVE_NOTACTIVE;
	}

	if( leaf->state[slot] == CHUNKSTATE_LOCKED ) {
		return ACQUIREACTIVE_ISLOCKED;
	}

	if( leaf->state[slot] == CHUNKSTATE_READY ) {
		leaf->state[slot] = CHUNKSTATE_SHARED;
		
	} else if( leaf->state[slot] == CHUNKSTATE_SHARED ) {
		
	} else {
		return ACQUIREACTIVE_UNKNOWNERROR;
	}
	
	if( address ) {
		*address = leaf->data[slot];
	}
	leaf->locks[slot]++;
	return ACQUIREACTIVE_SUCCESS;
}

//-------------------------------------------------------------------------------------------------
int ChunkTree::TryAcquireActiveSharedS( boost::uint64_t shuffled_coords, Chunk **address ) {
	boost::lock_guard<boost::mutex> lock(mut);
	return TryAcquireActiveSharedES( shuffled_coords, address );
}

//-------------------------------------------------------------------------------------------------
int ChunkTree::TryAcquireActiveShared( int x, int y, int z, Chunk **address ) {
	return TryAcquireActiveSharedS( ShuffleCoords( x, y, z ), address );
}

//-------------------------------------------------------------------------------------------------
int ChunkTree::TryAcquireActiveShared( boost::uint64_t index, Chunk **address ) {
	int x, y, z;
	UnpackCoords( index, x, y, z );
	return TryAcquireActiveShared( x, y, z, address );
}

//-------------------------------------------------------------------------------------------------
int ChunkTree::WaitAcquireActiveSharedS( boost::uint64_t shuffled_coords, Chunk **address ) {
	boost::unique_lock<boost::mutex> lock(mut);
	int result;
	while( true ) {
		result = TryAcquireActiveSharedES( shuffled_coords, address );
		if( result == ACQUIREACTIVE_ISLOCKED ) {
			cvar_lockchanges.wait(lock);
		} else if( result == ACQUIREACTIVE_NOTACTIVE || result == ACQUIREACTIVE_NOTACTIVE_EMPTY ) {
			return result;
		} else if( result == ACQUIREACTIVE_SUCCESS ) {
			return result;
		}
	}
}

//-------------------------------------------------------------------------------------------------
int ChunkTree::WaitAcquireActiveShared( int x, int y, int z, Chunk **address ) {
	return WaitAcquireActiveSharedS( ShuffleCoords( x, y, z ), address );
}

//-------------------------------------------------------------------------------------------------
int ChunkTree::WaitAcquireActiveShared( boost::uint64_t index, Chunk **address ) {
	int x, y, z;
	UnpackCoords( index, x, y, z );
	return WaitAcquireActiveShared( x, y, z, address );
}

//-------------------------------------------------------------------------------------------------
void ChunkTree::ReleaseSetPointerS( boost::uint64_t shuffled_coords, Chunk *address ) {
	Leaf *leaf;
	int slot;
	GetActiveLeafSlot( shuffled_coords, &leaf, &slot );

	{
		boost::lock_guard<boost::mutex> lock(mut);
		leaf->data[slot] = address;
		leaf->state[slot] = CHUNKSTATE_READY;

		cvar_lockchanges.notify_all();
		
	}
}

//-------------------------------------------------------------------------------------------------
void ChunkTree::ReleaseSetPointer( int x, int y, int z, Chunk *address ) {
	ReleaseSetPointerS( ShuffleCoords( x, y, z ), address );
}

//-------------------------------------------------------------------------------------------------
void ChunkTree::ReleaseSetPointer( boost::uint64_t index, Chunk *address ) {
	int x, y, z;
	UnpackCoords( index, x, y, z );
	ReleaseSetPointer( x, y, z, address );
}

//-------------------------------------------------------------------------------------------------
void ChunkTree::ReleaseSetInactiveS( boost::uint64_t shuffled_coords, int inactive_state ) {
	Leaf *leaf;
	int slot;
	GetActiveLeafSlot( shuffled_coords, &leaf, &slot );

	{
		boost::lock_guard<boost::mutex> lock(mut);
		leaf->data[slot] = 0;
		leaf->state[slot] = inactive_state;
		leaf->counter--;
		
		// <TODO: add to garbage list if counter == 0>
		if( leaf->counter == 0 ) {
			AddLeafToGarbage( leaf );
		}

		cvar_lockchanges.notify_all();
		
	}
}

//-------------------------------------------------------------------------------------------------
void ChunkTree::ReleaseSetInactive( int x, int y, int z, int inactive_state ) {
	ReleaseSetInactiveS( ShuffleCoords( x, y, z ), inactive_state );
}

//-------------------------------------------------------------------------------------------------
void ChunkTree::ReleaseSetInactive( boost::uint64_t index, int inactive_state ) {
	int x, y, z;
	UnpackCoords( index, x, y, z );
	ReleaseSetInactive( x, y, z, inactive_state );
}

//-------------------------------------------------------------------------------------------------
void ChunkTree::ReleaseSharedS( boost::uint64_t shuffled_coords ) {
	Leaf *leaf;
	int slot;
	GetActiveLeafSlot( shuffled_coords, &leaf, &slot );

	{
		boost::lock_guard<boost::mutex> lock(mut);
		leaf->locks[slot]--;
		if( leaf->locks[slot] == 0 ) {
			leaf->state[slot] = CHUNKSTATE_READY;
			cvar_lockchanges.notify_all();
			// notify waiting threads of change
		}
	}
}

//-------------------------------------------------------------------------------------------------
void ChunkTree::ReleaseShared( int x, int y, int z ) {
	ReleaseSharedS( ShuffleCoords( x, y, z ) );
}

//-------------------------------------------------------------------------------------------------
void ChunkTree::ReleaseShared( boost::uint64_t index ) {
	int x, y, z;
	UnpackCoords( index, x, y, z );
	ReleaseShared( x, y, z );
}

//-------------------------------------------------------------------------------------------------
int ChunkTree::QueryStateES( boost::uint64_t shuffled_coords ) {
	Leaf *leaf;
	int slot;
	if( !GetLeafSlot( shuffled_coords, &leaf, &slot ) ) return CHUNKSTATE_UNKNOWN;
	return leaf->state[slot];
}

//-------------------------------------------------------------------------------------------------
int ChunkTree::QueryStateS( boost::uint64_t shuffled_coords ) {
	boost::lock_guard<boost::mutex> lock(mut);
	return QueryStateES( shuffled_coords );
}

//-------------------------------------------------------------------------------------------------
int ChunkTree::QueryState( int x, int y, int z ) {
	return QueryStateS( ShuffleCoords( x, y, z ) );
}

//-------------------------------------------------------------------------------------------------
int ChunkTree::QueryState( boost::uint64_t index ) {
	int x, y, z;
	UnpackCoords( index, x, y, z );
	return QueryState( x, y, z );
}

//-------------------------------------------------------------------------------------------------
int ChunkTree::QueryStateE( int x, int y, int z ) {
	return QueryStateES( ShuffleCoords( x, y, z ) );
}

//-------------------------------------------------------------------------------------------------
int ChunkTree::QueryStateE( boost::uint64_t index ) {
	int x, y, z;
	UnpackCoords( index, x, y, z );
	return QueryStateE( x, y, z );
}

//-------------------------------------------------------------------------------------------------
void ChunkTree::AddLeafToGarbage( Leaf *leaf ) {
	boost::lock_guard<boost::mutex> lock(garbage_mutex);
	if( leaf->in_garbage_list ) {
		// already in garbage list
		return;
	}

	leaf->in_garbage_list = true;
	leaf->next = 0;
	
	if( inactive_leaf_last ) {
		inactive_leaf_last->next = leaf;
		inactive_leaf_last = leaf;

	} else {
		inactive_leaf_first = inactive_leaf_last = leaf;
	}

	leaf_garbage_memory_total += sizeof(Leaf);
	if( leaf_garbage_memory_total > leaf_garbage_memory_limit ) {
		cvar_garbage_limit.notify_all();
	}
}

//-------------------------------------------------------------------------------------------------
bool ChunkTree::DeleteInactiveLeafE() {
	if( !inactive_leaf_first ) return false;

	// remove leaf from list
	Leaf *leaf = inactive_leaf_first;
	inactive_leaf_first = inactive_leaf_first->next;
	if( !inactive_leaf_first ) inactive_leaf_last = 0;

	leaf_garbage_memory_total -= sizeof(Leaf);

	if( leaf->counter == 0 ) {
		DeleteLeaf(leaf);
	} else {
		// leaf became active and should be removed from this list
	}
	return true;
}

//-------------------------------------------------------------------------------------------------
void ChunkTree::RunCleanup() {

	boost::lock_guard<boost::mutex> lock(mut);

	for( int counter = 0; counter < leaf_cleanup_throttle; counter++ ) {

		if( !DeleteInactiveLeafE() ) break;
	}
}

//-------------------------------------------------------------------------------------------------
void ChunkTree::LeafCleanupThread() {

	boost::unique_lock<boost::mutex> lock(garbage_mutex);

	while( true ) {
		
		if( !leaf_cleanup_thread_active ) break;

		if( leaf_garbage_memory_total <= leaf_garbage_memory_limit ) {
			cvar_garbage_limit.wait(lock);
			
		} else {
			RunCleanup();
			if( leaf_garbage_memory_total > leaf_garbage_memory_limit ) {
				lock.unlock();
				boost::this_thread::sleep( boost::posix_time::seconds( 1 ) );
				lock.lock();
			}
		}
		
	}

}

/*
//-------------------------------------------------------------------------------------------------
void ChunkTree::SetActivePointer( int x, int y, int z, Chunk *address ) {
	// this function doesn't use the mutex
	// and assumes proper management of the branches
	//
	// (branches should not ever be destroyed if they have active or changing slots)
	//
	// this function assumes a valid changing slot
	//
	Branch *branch;
	int slot;
	GetActiveBranchSlot( x, y, z, &branch, &slot );
	void *original = InterlockedExchangePointer( branch->data[slot], address );
	assert( original == (void*)STATE_CHANGING );
}

//-------------------------------------------------------------------------------------------------
void ChunkTree::SetActivePointer( boost::uint64_t index, Chunk *address ) {
	int x, y, z;
	UnpackCoords( index, x, y, z );
	SetActivePointer( x, y, z, address );
}

//-------------------------------------------------------------------------------------------------
void ChunkTree::Lock() {
	mut.lock();
}

//-------------------------------------------------------------------------------------------------
void ChunkTree::Unlock() {
	mut.unlock();
}

//-------------------------------------------------------------------------------------------------
void ChunkTree::SetInactiveState( int x, int y, int z, int state ) {
	// tree must be locked during this function
	// this function must only be called if the chunk it is removing isn't locked
	//
	Branch *branch;
	int slot;

	GetActiveBranchSlot( x, y, z, &branch, &slot );
	assert( branch->data[slot] <= (void*)STATE_INACTIVE_LAST );
	branch->counter--;
	branch->data[slot] = (void*)state;
}

//-------------------------------------------------------------------------------------------------
void ChunkTree::SetInactiveState( boost::uint64_t index, int state ) {
	int x, y, z;
	UnpackCoords( index, x, y, z );
	SetInactiveState( x, y, z, state );
}

//-------------------------------------------------------------------------------------------------
int ChunkTree::GetStateE( int x, int y, int z, Chunk **pointer ) {

	if( pointer ) {
		*pointer = 0;
	}

	Branch *branch;
	int slot;
	if( GetBranchSlot( x, y, z, &branch, &slot ) ) {
		if( branch->data[slot] == (void*)CHUNKSTATE_READY ) {
			if( pointer ) {
				*pointer = (Chunk*)branch->data[slot];
			}
			return CHUNKSTATE_READY;
		} else {
			return (int)branch->data[slot];
		}
	}
	return CHUNKSTATE_UNKNOWN;
}

//-------------------------------------------------------------------------------------------------
int ChunkTree::GetStateE( boost::uint64_t index, Chunk **pointer ) {
	int x, y, z;
	UnpackCoords( index, x, y, z );
	GetStateE( x, y, z, pointer );
}

//-------------------------------------------------------------------------------------------------
int ChunkTree::GetState( int x, int y, int z, Chunk **pointer ) {
	boost::shared_lock<boost::mutex> lock(mut);
	return GetStateE( x, y, z, pointer );
}

//-------------------------------------------------------------------------------------------------
int ChunkTree::GetState( boost::uint64_t index, Chunk **pointer ) {
	int x, y, z;
	UnpackCoords( index, x, y, z );
	GetState( x, y, z, pointer );
}

*/
//-------------------------------------------------------------------------------------------------

	/*
//-------------------------------------------------------------------------------------------------
void ChunkTable::SetState( int index, chunk_state state ) {
	state_table[ index ] = state;
	InterlockedExchange( (volatile unsigned int*)address_table + index, (unsigned int)0 );
}

//-------------------------------------------------------------------------------------------------
void ChunkTable::SetState( int x, int y, int z, chunk_state state ) {
	SetState( IndexFromCoords(x,y,z), state );
}

//-------------------------------------------------------------------------------------------------
void ChunkTable::SetReadyState( int index, Chunk *address ) {
	InterlockedExchange( (volatile unsigned int*)address_table + index, (unsigned int)address );
	state_table[index] = CHUNK_READY;
}

//-------------------------------------------------------------------------------------------------
void ChunkTable::SetReadyState( int x, int y, int z, Chunk *address ) {
	SetReadyState( IndexFromCoords( x, y, z ), address );
} 

//-------------------------------------------------------------------------------------------------
void ChunkTable::Invalidate( int index ) {
	state_table[ index ] = CHUNK_INVALID;
	InterlockedExchange( (volatile unsigned int*)address_table + index, (unsigned int)0 );
}

//-------------------------------------------------------------------------------------------------
void ChunkTable::Invalidate( int x, int y, int z ) {
	Invalidate( IndexFromCoords(x, y, z) );
}

//-------------------------------------------------------------------------------------------------
chunk_state ChunkTable::GetState( int index ) {
	return state_table[index];
}

//-------------------------------------------------------------------------------------------------
chunk_state ChunkTable::GetState( int x, int y, int z ) {
	return GetState( IndexFromCoords( x, y, z ) );
}

//-------------------------------------------------------------------------------------------------
Chunk *ChunkTable::GetAddress( int index ) {
	return (Chunk*)address_table[index]; // todo check for error
}

//-------------------------------------------------------------------------------------------------
Chunk *ChunkTable::GetAddress( int x, int y, int z ) {
	return GetAddress( IndexFromCoords( x, y, z ) );
}

*/
 

/*
//-------------------------------------------------------------------------------------------------
void ChunkTable::Clean() {
	if( address_table ) delete[] address_table;
	if( state_table ) delete[] state_table;
	valid = false;
}
*/

/*
//-------------------------------------------------------------------------------------------------
ChunkTable::~ChunkTable() {
	Clean();
}
*/

/*
//-------------------------------------------------------------------------------------------------
void ChunkTable::Create( int p_width, int p_height, int p_length ) {
	Clean();

	width = p_width;
	height = p_height;
	length = p_length;

	state_table = new boost::uint8_t[width*height*length];
	address_table = new volatile Chunk*[width*height*length];
	

	Reset();

	valid = true;
}*/
/*
//-------------------------------------------------------------------------------------------------
static bool hexadecimal_digit( char a ) {
	a = toupper(a);
	if( (a >= '0' && a <= '9') || (a >= 'A' && a <= 'F') ) return true;
	return false;
}

//-------------------------------------------------------------------------------------------------
static int hexadecimal_digit_convert( char a ) {
	if(a >= '0' && a <= '9') return (int)(a - '0');
	if(a >= 'a' && a <= 'f') return (int)(a - 'a' + 10);
	if(a >= 'A' && a <= 'F') return (int)(a - 'A' + 10);
	return 0;
}

//-------------------------------------------------------------------------------------------------
int ChunkTable::ParseRegionChunkStates( const boost::filesystem::path &path ) {

	// parse path
	// /xz/xzy.rgn

	// todo: proper "/" conversion
	
	const char *path_string = path.string().c_str();
	int len = strlen(path_string);

	if( len < 3 ) return -1; // invalid path
	if( len < 10 ) return -2; // invalid path

	char path_chars[11]; // "/xz/xzy.rgn"
	for( int i = 0 ; i < 11; i++ ) {
		path_chars[i] = path_string[len-(11-i)];
	}

	if( path_chars[0] != '//' || path_chars[3] != '//' || path_chars[7] != '.' || path_chars[8] != 'r' || path_chars[9] != 'g' || path_chars[10] != 'n' ) {
		return -3; // invalid path formatting
	}

	if( !hexadecimal_digit(path_chars[1]) ) return -4;
	if( !hexadecimal_digit(path_chars[2]) ) return -4;
	if( !hexadecimal_digit(path_chars[4]) ) return -4;
	if( !hexadecimal_digit(path_chars[5]) ) return -4;
	if( !hexadecimal_digit(path_chars[6]) ) return -4;
	
	int location_x, location_y, location_z;
	location_x = hexadecimal_digit_convert( path_chars[1] ) * 16 + hexadecimal_digit_convert( path_chars[4] );
	location_z = hexadecimal_digit_convert( path_chars[2] ) * 16 + hexadecimal_digit_convert( path_chars[5] );
	location_y = hexadecimal_digit_convert( path_chars[6] );

	if( location_x * 4 > width || location_y * 4 > height || location_z * 4 > length ) {
		return -5; // region is out of range
	}

	if( boost::filesystem::file_size( path ) < (4 * 4*4*4) ) {
		return -6; // corrupt region file
	}
	
	BinaryFile f( path.string().c_str(), BinaryFile::MODE_READ );
	

	if( !f.IsOpen() ) {
		return -7; // fatal error, could not open region file!
		// todo: proper handling of this
	}

	boost::uint32_t address_table[4*4*4];
	f.ReadBytes( (boost::uint8_t*)address_table, 4*4*4*4 );

	f.Close();

	const boost::uint32_t *address_table_read = address_table;

	for( int y = 0; y < 4; y++ ) {
		for( int z = 0; z < 4; z++ ) {
			for( int x = 0; x < 4; x++ ) {
				
				int table_entry = address_table[x+y*16+z*4];
				int state = CHUNK_DORMANT;
				if( table_entry == 0 ) state = CHUNK_DORMANT;
				if( table_entry == 1 ) state = CHUNK_EMPTY;
				else state = CHUNK_DISK;
				state_table[ (location_x*4+x) + (location_z*4+z)*width + (location_y*4+y)*width*length ] = state;
				
			}
		}
	}

	return 0;
}

//-------------------------------------------------------------------------------------------------
void ChunkTable::DiskInitializationLoop( int level, const boost::filesystem::path &path ) {

	boost::filesystem::directory_iterator iter( path );

	while( iter != boost::filesystem::directory_iterator() ) {
		if( boost::filesystem::is_directory(*iter) ) {
			DiskInitializationLoop( level+1, *iter );
		} else if( boost::filesystem::is_regular_file(*iter) ) {
			if( level == 0 ) {
				// not expecting files here, skip
			} else {
				// expecting region files
				ParseRegionChunkStates( path ); // todo: this is a bug
			}
		} else {
			// unknown file, skip
		}
		iter++;
	}
	
}
*/
/*
//-------------------------------------------------------------------------------------------------
void ChunkTable::InitializeFromDisk( const char *path ) {

	DiskInitializationLoop( 0, path );
}
*/

/*
//-------------------------------------------------------------------------------------------------
void ChunkTable::Reset() {
	 
	for( int i = 0; i < width*height*length; i++ ) {
		state_table[i] = CHUNK_INVALID;
	}
}*/

}

#endif
