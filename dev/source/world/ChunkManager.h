//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//
#if 0

#pragma once


#include <boost/thread.hpp>

namespace World {


class Engine;

//------------------------------------------------------------------------------------------------------------------------
class ChunkManager {

	enum {
		DEFAULT_MEMORY_THRESHOLD_QUOTA = 500 * 1024 * 1024, // 500 mb
		DEFAULT_MEMORY_THRESHOLD_CLEAN = 550 * 1024 * 1024, // 550 mb
	};

	enum {
		MAX_CLEAN_JOBS = 16
	};

	// linked list
	Chunk *first;
	Chunk *last;

	boost::mutex guard;

	// extra data
	int total_chunks;

	// measured in bytes
	int memory_usage;				// current amount of memory
	int memory_threshold_quota;		// threshold at which to meet after a clean
	int memory_threshold_clean;		// threshold at which to start a clean

	void AddEntry( Chunk *ch );
	void DeleteEntry( Chunk *ch );
	void DeleteAllEntries();
	void BumpEntry( Chunk *ch );

	Engine *engine; // parent engine

public:

	ChunkManager();
	~ChunkManager();

	// allocate a new chunk
	//Chunk *Create();

	void UpdateChunkSize( Chunk *ch );
	
	// when memory usage exceeds the "clean" threshold, the clean function
	// will free memory until the memory_usage is under the "quota" threshold
	void SetMemoryQuotaThreshold( int bytes );
	void SetMemoryCleanThreshold( int bytes );

	// "bump" takes a chunk from the list and pushes it to the end
	// (as to avoid reallocation for it)
	void Bump( Chunk *ch );

	// push a chunk to the back
	void Push( Chunk *ch );

	// routine that trims the buffer back down under the quota size
	//
	// note: during a clean, chunk data may be released, so all pointers to chunk data are invalidated here.
	//
	void Clean();
};

}

#endif
