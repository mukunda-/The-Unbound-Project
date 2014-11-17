//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

#pragma once

#include "arenamanager.h"

//-----------------------------------------------------------------------------
namespace Mem { namespace Arena {

	/// -----------------------------------------------------------------------
	/// Allocated memory header.
	///
	struct Header { 
		uint16_t m_parent_id;
	};

	/// -----------------------------------------------------------------------
	/// An arena.
	///
	class Chunk {
		char m_data[CHUNK_SIZE];
		int  m_next = 0;
		int  m_references = 0;
		bool m_old = false;

		/// -------------------------------------------------------------------
		/// Delete this chunk.
		///
		void Finalize() {
		}

	public:
		~Chunk() {
			assert( m_references == 0 );
		}

		/// -------------------------------------------------------------------
		/// Allocate some memory from this chunk.
		///
		/// @param size    Size to allocate.
		/// @param aligned Align on this boundary. 0 = no alignment.
		/// @returns Pointer to allocated memory.
		///
		void *Get( int size, int aligned = 16 ) {
			int remaining = CHUNK_SIZE - m_next;
			if( remaining < (size + sizeof(Header)) ) return nullptr;

			void *ptr = (void*)(m_data + m_next);
			m_next += size + sizeof(Header);
		}

		/// -------------------------------------------------------------------
		/// Release one memory pointer.
		///
		/// This just decreases the reference counter, the chunk is freed
		/// when no more references are made.
		///
		void Release() {
			assert( m_references > 0 );
			m_references--;
			if( m_old && m_references == 0 ) {
				Finalize();
			}
		}

		/// -------------------------------------------------------------------
		/// Mark this chunk as "old", which means it will delete itself
		/// when all of its references are done.
		///
		void Old() {
			m_old = true;
			if( m_references == 0 ) Finalize();
		}
	};

}}
