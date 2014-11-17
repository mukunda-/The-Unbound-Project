//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

#pragma once 

//
// STL allocator implementation for arena allocation.
// idk what im doing here

#include "arena.h"

//-----------------------------------------------------------------------------
namespace Mem { namespace Arena {

	/*
	template <> class ArenaAllocator<void> {
	public:
		typedef void* pointer;
		typedef const void* const_pointer;
		typedef void value_type;
		template <class U> struct rebind { 
			typedef ArenaAllocator<U> other; 
		};
	};
	*/

	//-------------------------------------------------------------------------
	template< class T > class ArenaAllocator {
	public:
		typedef std::size_t size_type;
		typedef ptrdiff_t difference_type;
		typedef T* pointer;
		typedef const T* const_pointer;
		typedef T& reference;
		typedef const T& const_reference;
		typedef T value_type;
		template <class U> struct rebind { 
			typedef allocator<U> other; 
		};
 
		//---------------------------------------------------------------------
		explicit ArenaAllocator() throw() {} 
		explicit ArenaAllocator( const ArenaAllocator& ) throw() {}
		template< class U > 
		explicit ArenaAllocator( const ArenaAllocator<U>& ) throw() {}
		~ArenaAllocator() throw() {}
 
		//---------------------------------------------------------------------
		pointer address( reference x ) const { return x; }
		const_pointer address( const_reference x ) const { return x; }
 
		//---------------------------------------------------------------------
		pointer allocate( size_type count, 
						  allocator<void>::const_pointer = 0 ) {

			return (pointer)ArenaAlloc( count * sizeof(T) );
		}

		//---------------------------------------------------------------------
		void deallocate( pointer p, size_type n ) {
			ArenaFree( p );
		}

		//---------------------------------------------------------------------
		size_type max_size() const throw() {
			return ARENA_SIZE / sizeof(T);
		}

		//---------------------------------------------------------------------
		void construct( pointer p, const T& val ) {
			new((void*)p) T(val);
		}
		
		//---------------------------------------------------------------------
		void construct( pointer p ) {
			new((void*)p) T();
		}

		//---------------------------------------------------------------------
		void destroy( pointer p ) {
			p->~T();
		}

		//---------------------------------------------------------------------
		inline bool operator==(Allocator const&) { return true; }
		inline bool operator!=(Allocator const& a) { return !operator==(a); }
	};

}} // Mem::Arena
