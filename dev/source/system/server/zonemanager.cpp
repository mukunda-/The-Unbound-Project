//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//

//-------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "system/server/zonemanager.h"
#include "mem/memorylib.h"

//-------------------------------------------------------------------------------------------------
namespace System {

//-------------------------------------------------------------------------------------------------
// zone index bits:
// 0-3 = leaf X
// 4-6 = branch X
// 7-9 = trunk X
// 16-19 = leaf Y
// 20-22 = branch Y
// 23-25 = trunk Y

#define INDEX_TRUNK(m_zoneindex) (((m_zoneindex>>7)&7) + (((m_zoneindex>>23)&7)<<3))
#define INDEX_BRANCH(m_zoneindex) (((m_zoneindex>>4)&7) + (((m_zoneindex>>20)&7)<<3))
#define INDEX_LEAF(m_zoneindex) (((m_zoneindex)&15) + (((m_zoneindex>>16)&15)<<4))

//-------------------------------------------------------------------------------------------------
ZoneManager::ZoneManager() {
	memset( &trunk, 0, sizeof(trunk) );
	usage = 0;
}

//-------------------------------------------------------------------------------------------------
ZoneManager::~ZoneManager() {
	// clean up tree

	for( int a = 0; a < 8*8; a++ ) {
		Branch *branch = (Branch*)(trunk.data[a]);
		if( !branch ) continue;
		for( int b = 0; b < 8*8; b++ ) {
			Leaf *leaf = (Leaf*)(branch->data[b]);
			if( !leaf ) continue;
			Memory::FreeMem( leaf );
			branch->data[b] = 0;
		}
		Memory::FreeMem( trunk.data[a] );
		trunk.data[a] = 0;
	}
	// all clear
	
}

//-------------------------------------------------------------------------------------------------
ZoneManager::Branch *ZoneManager::CreateBranch( Branch *parent, int slot ) {
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
ZoneManager::Leaf *ZoneManager::CreateLeaf( Branch *parent, int slot ) {
	if( parent->data[slot] != 0 ) return (Leaf*)parent->data[slot];
	Leaf *b = (Leaf*)Memory::AllocMem( sizeof(Leaf) );
	usage++;
	memset( b, 0, sizeof(b) );
	b->parent = parent;
	b->slot = slot;
	parent->data[slot] = b;
	parent->counter++;
	return b;
}
//-------------------------------------------------------------------------------------------------
void ZoneManager::DeleteBranch( Branch *branch ) {
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
void ZoneManager::DeleteLeaf( Leaf *leaf ) {

	// make sure the leaf is not being used
	assert( leaf->references == 0 );
	//assert( leaf->zone_xlock == 0 );
	//assert( leaf->zone_shares == 0 );
	//assert( leaf->total_locks == 0 );

	Branch *parent = leaf->parent;
	int slot = leaf->slot;
	Memory::FreeMem( leaf );
	usage--;
	if( parent ) {
		parent->data[slot] = 0;
		parent->counter--;
		if( parent->counter == 0 ) {
			DeleteBranch( parent );
		}
	}
}

//-------------------------------------------------------------------------------------------------
void ZoneManager::GetLeafSlotOrCreate( ZONEINDEX index, Leaf **r_leaf, int *r_slot ) {
	Branch *branch;
	branch = CreateBranch( &trunk, INDEX_TRUNK(index) );
	Leaf *leaf = CreateLeaf( branch, INDEX_BRANCH(index) );
	

	if( r_leaf ) {
		*r_leaf = leaf;
	}

	if( r_slot ) {
		*r_slot = INDEX_LEAF(index);
	}

}

//-------------------------------------------------------------------------------------------------
bool ZoneManager::GetLeafSlot( ZONEINDEX index, Leaf **r_leaf, int *r_slot ) {
	Branch *branch = (Branch*)trunk.data[ INDEX_TRUNK(index) ];
	if( !branch ) return false;
	Leaf *leaf = (Leaf*)branch->data[ INDEX_BRANCH(index) ];
	if( !leaf ) return false;

	if( r_leaf ) {
		*r_leaf = leaf;
	}

	if( r_slot ) {
		*r_slot = INDEX_LEAF(index);
	}

	return true;
}

//-------------------------------------------------------------------------------------------------
bool ZoneManager::Map( ZONEINDEX index, int computer ) {
	assert( computer != 0 );
	return Remap( index, 0, computer );
}

//-------------------------------------------------------------------------------------------------
bool ZoneManager::Remap( ZONEINDEX index, int original, int computer ) {
	assert( computer != 0 );
	boost::lock_guard<boost::mutex> lock(mut);

	Leaf *leaf;
	int slot;
	GetLeafSlotOrCreate( index, &leaf, &slot );
	if( leaf->data[slot] != original ) return false;

	leaf->data[slot] = computer;
	
	if( original == 0 ) {
		leaf->references++;
	}
	return true;
}

//-------------------------------------------------------------------------------------------------
bool ZoneManager::Unmap( ZONEINDEX index ) {
	boost::lock_guard<boost::mutex> lock(mut);
	Leaf *leaf;
	int slot;
	if( !GetLeafSlot( index, &leaf, &slot ) ) return false;
	if( leaf->data[slot] == 0 ) return false;

	leaf->data[slot] = 0;
	leaf->references--;
	if( leaf->references == 0 ) {
		DeleteLeaf(leaf);
	}
	return true;
}

//-------------------------------------------------------------------------------------------------
int ZoneManager::Read( ZONEINDEX index ) {
	boost::lock_guard<boost::mutex> lock(mut);
	Leaf *leaf;
	int slot;
	if( !GetLeafSlot( index, &leaf, &slot ) ) return 0;
	return leaf->data[slot];
}

//-------------------------------------------------------------------------------------------------
}
