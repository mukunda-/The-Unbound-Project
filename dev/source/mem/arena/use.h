//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

#pragma once

#include "arena.h"

namespace Mem { namespace Arena {

	/// -----------------------------------------------------------------------
	/// Overrides new and delete to use the arena allocator.
	///
	class Use { 
		
	public:
		void* operator new( size_t size ) { 
			assert( size <= Chunk::SIZE );
			return Alloc( (int)size );
		} 

		void operator delete( void* ptr ) {
			Free( ptr );
		}
	};
}}
