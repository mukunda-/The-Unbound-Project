//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

#include <stdafx.h>
#include "arena.h"
#include "chunk.h"

namespace Mem { namespace Arena {

	//-------------------------------------------------------------------------
	Chunk::Chunk( Manager &manager, uint16_t id ) 
				: m_manager(manager), m_id(id) 
	{
	}
	
	//-------------------------------------------------------------------------
	Chunk::~Chunk() {
		assert( m_references == 0 ); // make sure we aren't accidentally 
		                            //  deleting chunks in use.
		
	}
	
	//-------------------------------------------------------------------------
	void *Chunk::Get( int size, int aligned ) {
		if( aligned < 2 ) aligned = 2;
			
		// reserve space for header
		int pos = m_free + sizeof( Header );

		// the address of m_data might not be aligned.
		int alignment_needed = 
			(aligned - (((int)m_data + pos) & (aligned-1))) & (aligned-1);

		// align as desired
		pos += alignment_needed;

		// cancel if we dont have enough space.
		int remaining = SIZE - pos;
		if( remaining < (size + (int)sizeof(Header)) ) {

			// mark for deletion after all references are released
			m_delete = true;
			if( m_references == 0 ) {
				m_manager.MemoryFreed( m_allocated );
				m_allocated = 0;
				m_manager.Finalize( this );
			}
			return nullptr;
		}

		void *ptr = (void*)(m_data + pos); 
		GetHeader( ptr )->m_id = m_id; 
		m_free = pos + size;
		m_references++; 

		m_allocated += size;

		return ptr;
	}
	
	//-------------------------------------------------------------------------
	void Chunk::Release() {
		assert( m_references > 0 );
		m_references--;

		// if no more references, reset allocated counter
		if( m_references == 0 ) {
			m_manager.MemoryFreed( m_allocated );
			m_allocated = 0;
		}

		// check marked for deletion
		if( m_delete && m_references == 0 ) {
			m_manager.Finalize( this ); 
			// remember that Finalize deletes "this"!
		}
	}
	
	//-------------------------------------------------------------------------
	void Chunk::Delete() {
		m_delete = true;
		if( m_references == 0 ) {
			m_manager.Finalize( this );
		}
	}
}}
