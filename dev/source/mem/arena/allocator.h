//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

#pragma once 

//
// STL allocator for the arena.
//

#include "arena.h"
#include "chunk.h"

//-----------------------------------------------------------------------------
namespace Mem { namespace Arena {
	 
	//-------------------------------------------------------------------------
	template< class T > class Allocator {
	public:
		using size_type = size_t;
		using difference_type = ptrdiff_t;
		using pointer = T*;
		using const_pointer = const T*;
		using reference = T&;
		using const_reference = const T&;
		using value_type = T;

		//---------------------------------------------------------------------
		template <class U> struct rebind { 
			typedef Allocator<U> other; 
		};
 
		//---------------------------------------------------------------------
		Allocator() throw() {} 
		Allocator( const Allocator& ) throw() {}
		template< class U > 
		Allocator( const Allocator<U>& ) throw() {}
		~Allocator() throw() {}
 
		//---------------------------------------------------------------------
		pointer address( reference x ) const { return &x; }
		const_pointer address( const_reference x ) const { return &x; }
 
		//---------------------------------------------------------------------
		pointer allocate( size_type count, 
						  const_pointer = 0 ) {
			if( count == 0 ) return nullptr;
			return (pointer)Alloc( (int)(count * sizeof(T)) );
		}

		//---------------------------------------------------------------------
		void deallocate( pointer p, size_type n ) {
			Free( p );
		}

		//---------------------------------------------------------------------
		size_type max_size() const {
			return Chunk::MaxSize() / sizeof(T);
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
	
	template <> class Allocator<void> {
	public:
		typedef void* pointer;
		typedef const void* const_pointer;
		typedef void value_type;
		template <class U> struct rebind { 
			typedef Allocator<U> other; 
		};
	};

}} // Mem::Arena
