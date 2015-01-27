//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

#pragma once

#include "allocator.h"

// Arena allocator
// for small short lived objects.

namespace Mem { namespace Arena {

	class Chunk;
	
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
	/// Allocate a shared pointer using the arena.
	///
	/// @param T    Object type to allocate and wrap.
	/// @param args Construction arguments.
	/// @returns    shared_ptr object.
	///
	template< class T, class... Args >
	std::shared_ptr<T> MakeShared( Args &&...args ) { 
		return std::allocate_shared<T>( Allocator<T>(), args... );
	}
	
	/// -----------------------------------------------------------------------
	/// Allocate a unique pointer using the arena.
	///
	/// @param T    Object type to allocate and wrap.
	/// @param args Construction arguments.
	/// @returns    unique_ptr object.
	///
	template< class T, class... Args >
	std::unique_ptr<T> MakeUnique( Args &&...args ) { 
		return std::unique_ptr<T>( new T(args...) );
	}

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
		 
		std::unordered_map< uint16_t, std::unique_ptr<Chunk> > m_chunkmap;
		
		uint16_t   m_nextid = 0;
		Chunk     *m_current_chunk = nullptr;
		std::mutex m_mut;

		// amount of memory currently allocated.
		int64_t    m_allocated = 0;

		// max amount of memory ever allocated at a time
		int64_t    m_allocated_peak = 0;
		
		void AllocateChunk(); 
		void Finalize( Chunk *chunk );
		void MemoryFreed( int amount );	

	public:
		Manager();
		~Manager();

		void *Alloc( int size, int aligned = 16 );
		void Free( void *ptr );
	};
	
}}
