//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2016, Mukunda Johnson, All rights reserved. ==========//

#pragma once

// the cell pool manages allocating and freeing cells from a global source (shared between world engines)
// 

//-------------------------------------------------------------------------------------------------
#include "world/Cell.h"
#include "world/JobManager.h"
#include <boost/thread.hpp>

//-------------------------------------------------------------------------------------------------
namespace World {

class Engine;

//-------------------------------------------------------------------------------------------------
class CellPool {
	
	//-----------------------------------------------------------------------
	enum {
		DEFAULT_MEMORY_QUOTA = 250 * 1024 * 1024, // 250 mb
	};

	//-----------------------------------------------------------------------
	Cell *first;
	Cell *last;

	boost::mutex mut;

	//-----------------------------------------------------------------------
	boost::mutex clean_thread_mutex;
	bool shutting_down;

	boost::thread clean_thread;
	boost::condition_variable cvar_clean_wakey;
	
	//-----------------------------------------------------------------------
	// memory statistics
	int total_cells;
	volatile boost::uint64_t memory_usage;		// number of bytes used
	boost::uint64_t memory_quota;		//

	Engine *engine; // parent engine

	//-----------------------------------------------------------------------
	void RemoveEntry( Cell *ch );
	void AddEntry( Cell *ch );

	int FreeCell();
	int DoFree( Cell *ch );
	void CleanThread();

	// compute the memory size for a cell and store it in recorded_size
	int CacheCellSize( Cell *ch );

	// add memory to the counter safely and wakes up the cleaner thread if the memory
	// exceeds quota
	void AddMemory( Cell *ch );
	// AddMemory subfunction
	void WakeCleanTest();

	// pop a cell for deletion testing
	Cell * PopFront();

	// callback for saving the cell for deletion
	static void OnSaveComplete( Job *job );

	//-----------------------------------------------------------------------
public:
	
	CellPool();
	~CellPool();

	//------------------------------------------------------------------
	// set memory quota, in bytes
	//
	// memory quota is the limit allowed for cell memory
	// older cells will automatically be deleted if the memory quota is
	// exceeded
	//
	void SetQuota( boost::uint64_t quota );

	//------------------------------------------------------------------
	// bump a cell to the front of the list to avoid it being targetted
	// by the purging function
	//
	void Bump( Cell *ch );

	//------------------------------------------------------------------
	// push a new cell into the pool
	//
	void Push( Cell *ch );

	//------------------------------------------------------------------
	// delete a cell from the pool (for replacement, does not save the data)
	//
	void Delete( Cell *ch );
	
	//------------------------------------------------------------------
	// call when the memory size of the cell has changed (must already exist in the pool)
	// use this before the cell is released
	//
	void MemorySizeChanged( Cell *ch );

	//------------------------------------------------------------------
	// save and delete all cells...
	// 
	void Flush();
};

//-------------------------------------------------------------------------------------------------
}

