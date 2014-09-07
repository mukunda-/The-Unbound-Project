//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//

#include "stdafx.h"

#if 0
//-------------------------------------------------------------------------------------------------


namespace World {
//-------------------------------------------------------------------------------------------------
ChunkManager::ChunkManager() {
	memory_usage = 0;
	first = last = 0;
	total_chunks = 0;
	engine = 0;
	
	SetMemoryQuotaThreshold( DEFAULT_MEMORY_THRESHOLD_QUOTA );
	SetMemoryCleanThreshold( DEFAULT_MEMORY_THRESHOLD_CLEAN );
}

//-------------------------------------------------------------------------------------------------
ChunkManager::~ChunkManager() {
	DeleteAllEntries();
}

//-------------------------------------------------------------------------------------------------
void ChunkManager::SetMemoryQuotaThreshold( int p ) {
	memory_threshold_quota = p;
}

//-------------------------------------------------------------------------------------------------
void ChunkManager::SetMemoryCleanThreshold( int p ) {
	memory_threshold_clean = p;
}

//-------------------------------------------------------------------------------------------------
void ChunkManager::AddEntry( Chunk *ch ) {
	
	ch->next = 0;

	// lock structure, increase memory values and append item to list
	boost::lock_guard<boost::mutex> lock(guard);
	//memory_usage += Memory::SizeOf( ch );
	
	memory_usage += ComputeChunkSize( ch );

	total_chunks++;

	if( !last ) {
		first = last = ch;
		ch->prev = 0;
	} else {
		last->next = ch;
		ch->prev = last;
		last = ch;
	}
}

//-------------------------------------------------------------------------------------------------
void ChunkManager::DeleteEntry( Chunk *ch ) {
	// note: ONLY called from Clean()
	{
		boost::lock_guard<boost::mutex> lock(guard);
		memory_usage -= Memory::SizeOf( ch );
		total_chunks--;

		if( ch->compressed ) memory_usage -= Memory::SizeOf( ch->compressed );
		//if( ch->paint ) memory_usage -= Memory::SizeOf( ch->paint );
		//if( ch->userdata ) memory_usage -= Memory::SizeOf( ch->userdata );
	
		if( ch->next )
			ch->next->prev = ch->prev;
		else
			last = ch->prev;

		if( ch->prev )
			ch->prev->next = ch->next;
		else
			first = ch->next;
	}
	//Memory::FreeMem( ch->paint );
	//Memory::FreeMem( ch->userdata );
	Memory::FreeMem( ch->compressed );
	Memory::FreeMem( ch );
}

//-------------------------------------------------------------------------------------------------
void ChunkManager::DeleteAllEntries() {
	Chunk *first_copy, *last_copy;
	{
		boost::lock_guard<boost::mutex> lock(guard);
		first_copy = first;
		last_copy = last;
		first = last = 0;
		memory_usage = 0;
		total_chunks = 0;
	}

	for( Chunk *ch = first_copy; ch; ch = ch->next ) {
		//Memory::FreeMem( ch->paint );
		//Memory::FreeMem( ch->userdata );
		Memory::FreeMem( ch->compressed );
		Memory::FreeMem( ch );
	}
}

//-------------------------------------------------------------------------------------------------
void ChunkManager::BumpEntry( Chunk *ch ) {
	boost::lock_guard<boost::mutex> lock(guard);

	// remove from list
	if( ch->next )
		ch->next->prev = ch->prev;
	else
		last = ch->prev;

	if( ch->prev )
		ch->prev->next = ch->next;
	else
		first = ch->next;

	// add to list
	ch->next = 0;

	if( !last ) {
		first = last = ch;
		ch->prev = 0;
	} else {
		last->next = ch;
		ch->prev = last;
		last = ch;
	}

}
/*
//-------------------------------------------------------------------------------------------------
Chunk *ChunkManager::Create() {

	Chunk *ch = (Chunk*)Memory::AllocMem( sizeof(Chunk) );

	ch->compressed = 0;
	//ch->paint = 0;
	//ch->userdata = 0;


	//AddEntry( ch ); cant add here, chunk must be actually initialized
	// because the clean function might catch it
}*/

//-------------------------------------------------------------------------------------------------
void ChunkManager::UpdateChunkSize( Chunk *ch ) {
	boost::lock_guard<boost::mutex> lock(guard);

	memory_usage -= ch->recorded_size;
	memory_usage += ComputeChunkSize(ch);
}

//-------------------------------------------------------------------------------------------------
void ChunkManager::Bump( Chunk *ch ) {
	BumpEntry( ch );
}

//-------------------------------------------------------------------------------------------------
void ChunkManager::Push( Chunk *ch ) {
	AddEntry( ch );
}

//-------------------------------------------------------------------------------------------------
void ChunkManager::Clean() {
	//
	// NOTE: during a clean, the chunk data can NOT be safely accessed
	//       any threads involving chunk data must be suspended during a clean
	//
	Chunk *first_copy, *last_copy;

	{
		boost::lock_guard<boost::mutex> lock(guard);
		if( !first ) return; // no chunks allocated

		// unlink list
		first_copy = first;
		last_copy = last;
		first = last = 0;
	}

	for( Chunk *ch = first_copy; ; ) {

		if( ch->changed ) {
			if( engine->GetJobs()->GetPendingJobs() >= MAX_CLEAN_JOBS ) {
				break;
			}
		}

		if( ch->changed ) {
			engine->PushJob_ChunkSave( ch );
			
		}

		// don't iterate past last copy (new ones could have been added which are uninitialized)
		if( ch == last_copy ) {
			break;
		} 
	}

}

}

#endif
