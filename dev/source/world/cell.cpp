//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//

#include "stdafx.h"

namespace World {
	
//-------------------------------------------------------------------------------------------------
void InitializeCellHeader( Cell *ch ) {
	ch->ct = 0;
	ch->world_index = 0;
	ch->shuffled_index = 0;
	ch->render_flags = 0;
	ch->owner = 0;
	ch->group = 0;
	//ch->busy = 0;
	ch->changed = 0;
	/*
	for( int i = 0; i < CDATA_TOTAL; i++ ) {
		ch->data_loaded[i] = 0;
	}*/

	for( int i = 0; i < COMP_TOTAL; i++ ) {
		ch->comp_sizes[i] = 0;
	}

	ch->prev = 0;
	ch->next = 0;

	ch->compressed = 0;
	 
	ch->codec_cache = 0;
}
/*
Chunk::Chunk() {
	InitializeChunkHeader( this );
}

Chunk::~Chunk() {
}
*/


int ComputeCellSize( Cell *ch ) {
	int size = Memory::SizeOf( ch );
	size += Memory::SizeOf( ch->compressed );
	ch->recorded_size = size;
	return size;
}

}

