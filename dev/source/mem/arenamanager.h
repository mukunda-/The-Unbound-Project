//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

#pragma once

// Arena allocator

// Use for TEMPORARY objects, if an object does not have a definite short
// lifespan, do not use the arena allocator!

namespace Mem { namespace Arena {
	
	constexpr int CHUNK_SIZE = 0x100000; //1 megabyte.

	/// -----------------------------------------------------------------------
	/// Singleton object that manages arena memory allocation.
	///
	class Manager {
			
		std::list< std::unique_ptr<Chunk> > m_chunks;
		Chunk *current;

	public:
		Manager();
		~Manager();
	};
	
}}
