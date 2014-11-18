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
		assert( m_references == 0 );
	}
	
	//-------------------------------------------------------------------------
	void *Chunk::Get( int size, int aligned ) {
		if( aligned < 2 ) aligned = 2;
			
		// reserve space for header
		int pos = m_free + sizeof( Header );

		// align to desired
		pos = ((pos-1)|(aligned-1))+1;

		// cancel if we dont have enough space.
		int remaining = SIZE - pos;
		if( remaining < (size + (int)sizeof(Header)) ) {
			m_delete = true;
			if( m_references == 0 ) {
				m_manager.Finalize( this );
			}
			return nullptr;
		}

		void *ptr = (void*)(m_data + pos); 
		GetHeader( ptr )->m_id = m_id; 
		m_free = pos + size;
		m_references++; 
		return ptr;
	}
	
	//-------------------------------------------------------------------------
	void Chunk::Release() {
		assert( m_references > 0 );
		m_references--;
		if( m_delete && m_references == 0 ) {
			m_manager.Finalize( this );
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