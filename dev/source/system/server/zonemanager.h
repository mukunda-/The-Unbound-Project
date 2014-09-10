//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//

//---------------------------------------------------------------------------------------------
#pragma once

// ZoneManager manages and maps zones to computers

//---------------------------------------------------------------------------------------------
#include <boost/thread.hpp>

//---------------------------------------------------------------------------------------------
namespace System {

typedef unsigned int ZONEINDEX;

//---------------------------------------------------------------------------------------------
class ZoneManager {

	// quadtree
	//---------------------------------------------------------------------------------------------
	typedef struct t_Branch {
		void *data[8*8];
		int counter;

		struct t_Branch *parent;
		int slot;
	} Branch;

	//---------------------------------------------------------------------------------------------
	typedef struct t_Leaf {
		int data[16*16];

		int references;

		struct t_Branch *parent;
		int slot;
	} Leaf;

	Branch trunk;
	int usage;

	boost::mutex mut;

	Branch *CreateBranch( Branch *parent, int slot );
	Leaf *CreateLeaf( Branch *parent, int slot );
	void GetLeafSlotOrCreate( ZONEINDEX index, Leaf **r_leaf, int *r_slot );
	bool GetLeafSlot( ZONEINDEX index, Leaf **r_leaf, int *r_slot );
	void DeleteBranch( Branch *branch );
	void DeleteLeaf( Leaf *leaf );
	
	//---------------------------------------------------------------------------------------------
public:
	
	ZoneManager();

	//---------------------------------------------------------------------------------------------
	~ZoneManager();

	//---------------------------------------------------------------------------------------------
	// returns false if computer is already set
	bool Map( ZONEINDEX index, int computer );

	//---------------------------------------------------------------------------------------------
	// returns false if computer is not equal to original before new assignment
	//
	bool Remap( ZONEINDEX index, int original, int computer );

	//---------------------------------------------------------------------------------------------
	// returns false if the slot is not mapped
	bool Unmap( ZONEINDEX index );

	//---------------------------------------------------------------------------------------------
	// returns what computer is mapped to a certain index
	// 0 if none
	int Read( ZONEINDEX index );

	//---------------------------------------------------------------------------------------------
	static ZONEINDEX IndexFromCoords( int x, int y ) {
		return x + (y << 16);
	}
};

}
