//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//

#include "stdafx.h"

namespace World {

//----------------------------------------------------------------------------------
LockTree::LockTree() {
	memset( &trunk, 0, sizeof(trunk) );
}

//----------------------------------------------------------------------------------
LockTree::~LockTree() {
	
}

//----------------------------------------------------------------------------------
LockTree::Branch *LockTree::AddBranch( Branch *parent, int slot ) {
	Branch *b = (Branch*)Memory::AllocMem( sizeof(Branch) );
	memset( b, 0, sizeof(b) );
	b->parent = parent;
	b->slot = slot;
	parent->data[slot] = b;
	parent->counter++;
	return b;
}

//----------------------------------------------------------------------------------
LockTree::Leaf *LockTree::AddLeaf( Branch *parent, int slot ) {
	Leaf *leaf = (Leaf*)Memory::AllocMem( sizeof(Leaf) );
	memset( leaf, 0, sizeof(leaf) );
	leaf->parent = parent;
	leaf->slot = slot;
	parent->data[slot] = leaf;
	parent->counter++;
	return leaf;
}

//----------------------------------------------------------------------------------
void LockTree::DeleteBranch( Branch *b ) {
	if( b == &trunk ) return; // dont delete the trunk
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

//----------------------------------------------------------------------------------
void LockTree::DeleteLeaf( Leaf *leaf ) {
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

//----------------------------------------------------------------------------------
boost::uint64_t LockTree::PackCoords( int x, int y, int z ) const {
	return	( (x&15)+(y&7)*256+(z&15)*16 ) +
			(( ((x>>4)&15)+((y>>3)&7)*256+((z>>4)&15)*16)<<12) +
			((boost::uint64_t)( ((x>>8)&15)+((y>>6)&7)*256+((z>>8)&15)*16)<<32) +
			((boost::uint64_t)( ((x>>12)&15)+((y>>9)&7)*256+((z>>12)&15)*16)<<44);
}

//----------------------------------------------------------------------------------
bool LockTree::LockExists( boost::uint64_t coords ) const {
	if( !trunk.data[(coords>>44)&2047] ) return false;
	Branch *b = (Branch*)trunk.data[(coords>>44)&2047];
	if( !b->data[(coords>>32)&2047] ) return false;
	b = (Branch*)b->data[(coords>>32)&2047];
	if( !b->data[(coords>>12)&2047] ) return false;
	Leaf *leaf = (Leaf*)b->data[(coords>>12)&2047];
	return leaf->data[coords&2047] > 0;
}

//----------------------------------------------------------------------------------
void LockTree::CreateLock( boost::uint64_t coords ) {
	if( !trunk.data[(coords>>44)&2047] )
		AddBranch( &trunk, (coords>>44)&2047 );
	Branch *b = (Branch*)trunk.data[(coords>>44)&2047];

	if( !b->data[(coords>>32)&2047] )
		AddBranch( b, (coords>>32)&2047 );
	b = (Branch*)b->data[(coords>>32)&2047];

	if( !b->data[(coords>>12)&2047] )
		AddLeaf( b, (coords>>12)&2047 );
	Leaf *leaf = (Leaf*)b->data[(coords>>12)&2047];
	
	assert( leaf->data[coords&2047] == 0 );
	leaf->data[coords&2047] = 1;
	leaf->counter++;
}

//----------------------------------------------------------------------------------
void LockTree::RemoveLock( boost::uint64_t coords ) {
	assert( trunk.data[(coords>>44)] );
	Branch *b = (Branch*)trunk.data[(coords>>44)&2047];

	assert( b->data[(coords>>32)&2047] );
	b = (Branch*)b->data[(coords>>32)&2047];

	assert( b->data[(coords>>12)&2047] );
	Leaf *leaf = (Leaf*)b->data[(coords>>12)&2047];

	assert( leaf->data[coords&2047] );
	leaf->data[coords&2047] = 0;
	leaf->counter--;
	if( leaf->counter == 0 ) {
		DeleteLeaf(leaf);
	}
}

//----------------------------------------------------------------------------------
void LockTree::Acquire( boost::uint64_t coords ) {
	boost::unique_lock<boost::mutex> lock(cvar_mutex);

	while( LockExists( coords ) ) {
		cvar.wait(lock);
	}

	CreateLock( coords );
}

//----------------------------------------------------------------------------------
void LockTree::Acquire( int x, int y, int z ) {
	Acquire( PackCoords( x, y, z ) );
}

//----------------------------------------------------------------------------------
void LockTree::Release( boost::uint64_t coords ) {
	{
		boost::lock_guard<boost::mutex> lock(cvar_mutex);
		RemoveLock( coords );
	}

	cvar.notify_all();
}

//----------------------------------------------------------------------------------
void LockTree::Release( int x, int y, int z ) {
	Release( PackCoords(x,y,z) );
}

//----------------------------------------------------------------------------------
}


/*
//----------------------------------------------------------------------------------
void LockTable::Delete() {
	if( table ) {
		delete[] table;
		table = 0;
	}
}

//----------------------------------------------------------------------------------
void LockTable::Create( int size ) {
	Delete();

	// todo: this table needs to be aligned
	// (probably not a problem)
	table = new volatile boost::uint8_t[size];

	memset( (void*)table, 0, size );
}
*/