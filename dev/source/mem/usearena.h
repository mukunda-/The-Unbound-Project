//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

#pragma once

namespace Mem {

	namespace Arena {
		/// -------------------------------------------------------------------
		/// Overrides new and delete to use the arena allocator.
		///
		class Use { 

			void* operator new( size_t size ) { 
				return ArenaAlloc( sz );
			} 

			void operator delete( void* ptr ) {
				ArenaFree( ptr );
			}
		};
	}
}