//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//

#pragma once

//------------------------------------------------------------------------------------------------------------------------
#include <boost/cstdint.hpp>
#include <boost/thread.hpp>

//------------------------------------------------------------------------------------------------------------------------
namespace World {
	
//------------------------------------------------------------------------------------------------------------------------
class LockTree {
private:

	//-----------------------------------------------------------------------------------
	typedef struct t_Branch {
		void *data[16*16*8];
		int counter;
		struct t_Branch *parent;
		int slot;
	} Branch;

	typedef struct t_Leaf {
		boost::uint8_t data[16*16*8];
		int counter;
		Branch *parent;
		int slot;
	} Leaf;

	Branch trunk;

	boost::condition_variable cvar;
	boost::mutex cvar_mutex;

	Branch *AddBranch( Branch *parent, int slot );
	Leaf *AddLeaf( Branch *parent, int slot );
	void DeleteBranch( Branch *b );
	void DeleteLeaf( Leaf *leaf );

	bool LockExists( boost::uint64_t coords ) const;
	void CreateLock( boost::uint64_t coords );
	void RemoveLock( boost::uint64_t coords );

public:

	//-----------------------------------------------------------------------------------
	LockTree();
	~LockTree();
	
	//-----------------------------------------------------------------------------------
	// acquire a lock on a table entry
	// this function will block until the table entry is available (if not already)
	//
	// warning: deadlock if nested use within the same thread
	// (will also deadlock if another thread never releases a lock)
	//
	void Acquire( boost::uint64_t coords );
	void Acquire( int x, int y, int z );

	//-----------------------------------------------------------------------------------
	// release a table entry
	// warning: this must not be called if a lock wasnt acquired.
	//
	void Release( boost::uint64_t coords );
	void Release( int x, int y, int z );
	
	boost::uint64_t LockTree::PackCoords( int x, int y, int z ) const;
};

}
