//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

#include <stdafx.h>
#include "arena.h"
#include "chunk.h"

namespace Mem { namespace Arena {

	using Lock = std::lock_guard<std::mutex>;

	//-------------------------------------------------------------------------
	namespace {
		Manager *g_manager = nullptr;
	}

	//-------------------------------------------------------------------------
	void *Alloc( int size, int aligned ) {
		assert( g_manager );
		return g_manager->Alloc( size, aligned );
	}

	//-------------------------------------------------------------------------
	void Free( void *ptr ) {
		assert( g_manager );
		return g_manager->Free( ptr );
	}

	//-------------------------------------------------------------------------
	void *Manager::Alloc( int size, int aligned ) {
		if( size > Chunk::MaxSize() ) {
			throw std::length_error( "Request for arena memory too big." );
		}

		Lock guard(m_mut);

		void *ptr = m_current_chunk->Get( size, aligned );

		if( !ptr ) { 
			AllocateChunk();
			ptr = m_current_chunk->Get( size, aligned );
		}
		
		if( !ptr ) {
			throw std::runtime_error( "Error allocating from arena." );
		}

		m_allocated += size;

		return ptr;
	}

	//-------------------------------------------------------------------------
	void Manager::Free( void* ptr ) {
		Lock guard(m_mut);
		Header *header = Chunk::GetHeader( ptr );
		std::unique_ptr<Chunk> &chunk = m_chunkmap[ header->m_id ];
		chunk->Release();
	}

	/// -----------------------------------------------------------------------
	/// Allocate a memory chunk and register it.
	///
	void Manager::AllocateChunk() {
		m_current_chunk = new Chunk( *this, m_nextid ); 
		if( m_chunkmap.count( m_nextid ) ) {
			throw std::runtime_error( "Too many arenas allocated!?" );
		}
		
		m_chunkmap[m_nextid] = std::unique_ptr<Chunk>(m_current_chunk);
		m_nextid++;
	}
	
	/// -----------------------------------------------------------------------
	/// Called by chunks to remove themselves.
	///
	void Manager::Finalize( Chunk *chunk ) {
		m_chunkmap.erase( chunk->m_id );
	}

	//-------------------------------------------------------------------------
	void Manager::MemoryFreed( int amount ) {
		m_allocated -= amount;
	}

	//-------------------------------------------------------------------------
	Manager::Manager() {
		if( g_manager ) {
			throw std::runtime_error( 
				"Cannot create multiple arena managers." );
		}
		g_manager = this; 
		AllocateChunk();
	}

	//-------------------------------------------------------------------------
	Manager::~Manager() {
		if( m_allocated != 0 ) {

			// make sure there are no leaks.
			throw std::runtime_error( 
				"Arena contains allocated memory during destruction!" );
		}

		// m_current_chunk is deleted with m_chunkmap.
		
		g_manager = nullptr;
	}

}}
