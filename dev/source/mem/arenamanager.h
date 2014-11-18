//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

#pragma once

#include "util/linkedlist.h"
#include "arena.h"

// Arena allocator
// for small short lived objects.

namespace Mem { namespace Arena {
	
	/// -----------------------------------------------------------------------
	/// Allocate memory from an arena.
	///
	/// Use for SMALL,TEMPORARY objects, if an object does not have a definite 
	/// short lifespan, do not use the arena allocator! 
	///
	/// @param size Desired size in bytes.
	/// @param aligned If nonzero, align the memory to this many bytes.
	///
	void *Alloc( int size, int aligned = 16 );

	/// -----------------------------------------------------------------------
	/// Free allocated memory.
	///
	/// @param ptr Pointer received from Alloc
	///
	void Free( void *ptr );

	/// -----------------------------------------------------------------------
	/// Singleton object that manages arena memory allocation.
	///
	class Manager {
		friend class Chunk;
		 
		std::unordered_map< uint16_t, Chunk* > m_chunkmap;
		Util::LinkedList<Chunk> m_chunks;
		
		uint16_t   m_nextid = 0;
		Chunk     *m_current_chunk = nullptr;
		std::mutex m_mut;

		void AllocateChunk(); 
		void Finalize( Chunk *chunk );

	public:
		Manager();
		~Manager();

		void *Alloc( int size, int aligned = 16 );
		void Free( void *ptr );
	};
	
}}
