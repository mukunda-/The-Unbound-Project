//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

#pragma once


//-----------------------------------------------------------------------------
namespace Mem { namespace Arena { 

	/// -----------------------------------------------------------------------
	/// Allocated memory header.
	///
	struct Header { 
		uint16_t m_id;
	};

	class Manager;

	/// -----------------------------------------------------------------------
	/// An arena.
	///
	class Chunk : public Util::LinkedItem<Chunk> {
		friend class Manager;
		static const int SIZE = 0x1000000;

		Manager &m_manager;        // parent manager
		char     m_data[SIZE];     // arena memory block
		int      m_free = 0;       // next byte position of free data
		int      m_references = 0; // # of allocated units
		uint16_t m_id;             // ID in manager
		bool     m_delete = false; // delete when references reach 0

		/// -------------------------------------------------------------------
		/// Get the header for an allocated memory unit.
		///
		/// @param ptr Pointer obtained from Get
		/// @returns   Pointer to header data.
		///
		static Header *GetHeader( void *ptr ) {
			return (Header*)(
				((char*)ptr) - sizeof(Header)
			);
		}

	public:
		Chunk( Manager &manager, uint16_t id );
		~Chunk();

		/// -------------------------------------------------------------------
		/// Allocate some memory from this chunk.
		///
		/// @param size    Size to allocate.
		/// @param aligned Align on this boundary. 2 is minimum. 0 = 2
		///                aligned must be a power of 2.
		/// @returns Pointer to allocated memory.
		///
		void *Get( int size, int aligned = 16 );

		/// -------------------------------------------------------------------
		/// Release one memory pointer.
		///
		/// This decreases the reference counter, the chunk is freed
		/// when no more references are made.
		///
		void Release();

		/// -------------------------------------------------------------------
		/// Mark this chunk for deletion, which means it will signal
		/// the manager to be removed when all references are finished.
		///
		void Delete();

		/// -------------------------------------------------------------------
		/// Returns the max allowable size for allocation.
		///
		static const int MaxSize() {
			return SIZE*3/4;
		}
	};

}}
