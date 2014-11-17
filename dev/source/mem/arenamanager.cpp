//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

#include <stdafx.h>

namespace Mem {

	ArenaManager *g_arena_manager = nullptr;

	//-------------------------------------------------------------------------
	ArenaManager::ArenaManager() {
		if( g_arena_manager ) {
			throw std::runtime_error( 
				"Cannot create multiple arena managers." );
		}
		g_arena_manager = this;

	}

	//-------------------------------------------------------------------------
	ArenaManager::~ArenaManager() {
		// wait for memory to be unused.
		
		g_arena_manager = nullptr;
	}
}
