//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//

#include <stdafx.h>

//-------------------------------------------------------------------------------------------------
namespace World {

//-------------------------------------------------------------------------------------------------
CellPool::CellPool() {
	memory_usage = 0;
	first = last = 0;
	total_cells = 0;
	engine = 0;
	SetQuota( DEFAULT_MEMORY_QUOTA );
	shutting_down = false;
	clean_thread = boost::thread( boost::bind( &CellPool::CleanThread, this ) );

}

//-------------------------------------------------------------------------------------------------
CellPool::~CellPool() {
	Flush();
	{
		boost::lock_guard<boost::mutex> lock(mut);
		shutting_down = true;
	}
	cvar_clean_wakey.notify_all();
	clean_thread.join();

}

//-------------------------------------------------------------------------------------------------
void CellPool::Flush() {
	boost::lock_guard<boost::mutex> lock(clean_thread_mutex);
	Cell *ch;
	

	while( ch = PopFront() ) {
		// todo
		CellTree *cm = ch->ct->GetCells();
		cm->ReserveS( ( ch->shuffled_index ) );
		memory_usage -= DoFree( ch );
	}
}

//-------------------------------------------------------------------------------------------------
void CellPool::SetQuota( boost::uint64_t quota ) {
	memory_quota = quota;
}

//-------------------------------------------------------------------------------------------------
void CellPool::RemoveEntry( Cell *ch ) {
	if( ch->next )
		ch->next->prev = ch->prev;
	else
		last = ch->prev;

	if( ch->prev )
		ch->prev->next = ch->next;
	else
		first = ch->next;

	ch->next = 0;
	ch->prev = 0;

}

//-------------------------------------------------------------------------------------------------
void CellPool::AddEntry( Cell *ch ) {
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

//-------------------------------------------------------------------------------------------------
void CellPool::Bump( Cell *ch ) {
	boost::lock_guard<boost::mutex> lock(mut);
	RemoveEntry(ch);
	AddEntry(ch);
}

//-------------------------------------------------------------------------------------------------
void CellPool::WakeCleanTest() {
	if( memory_usage >= memory_quota ) {
		cvar_clean_wakey.notify_all();
	}
}

//-------------------------------------------------------------------------------------------------
void CellPool::AddMemory( Cell *ch ) {
	boost::lock_guard<boost::mutex> lock(clean_thread_mutex);
	memory_usage += ch->recorded_size;
	WakeCleanTest();
}

//-------------------------------------------------------------------------------------------------
void CellPool::MemorySizeChanged( Cell *ch ) {
	int old_size = ch->recorded_size;
	CacheCellSize(ch);
	
	{
		boost::lock_guard<boost::mutex> lock(clean_thread_mutex);
		memory_usage -= old_size;
		memory_usage += ch->recorded_size;
		WakeCleanTest();
	}
}

//-------------------------------------------------------------------------------------------------
void CellPool::Push( Cell *ch ) {
	boost::lock_guard<boost::mutex> lock(mut);
	AddEntry(ch);
	
	CacheCellSize( ch );
	AddMemory( ch );
}

//-------------------------------------------------------------------------------------------------
int CellPool::CacheCellSize( Cell *ch ) {
	int size = sizeof(Cell);
	for( int i = 0; i < COMP_TOTAL; i++ ) {
		size += ch->comp_sizes[i];
	}
	ch->recorded_size = size;
	return size;
}

//-------------------------------------------------------------------------------------------------
void CellPool::Delete( Cell *ch ) {
	boost::lock_guard<boost::mutex> lock(mut);
	RemoveEntry(ch);

	memory_usage -= ch->recorded_size;
	
	Memory::FreeMem( ch );
}

//-------------------------------------------------------------------------------------------------
Cell * CellPool::PopFront() {
	boost::lock_guard<boost::mutex> lock(mut);
	if( !first ) return 0;
	Cell *ch = first;
	RemoveEntry(ch);
	return ch;
}

//-------------------------------------------------------------------------------------------------
void CellPool::OnSaveComplete( Job *job ) {
	
	Cell *cell = (Cell*)job->userdata_ptr;
	CellTree *cm = cell->ct->GetCells();

	cm->UpgradeS( cell->shuffled_index );
	cm->SetUnknownCellStateS( cell->shuffled_index, CELLSTATE_DISK );
	cm->CompleteS( cell->shuffled_index );

	Memory::FreeMem( cell );
}

//-------------------------------------------------------------------------------------------------
int CellPool::DoFree( Cell *ch ) {
	CellTree *cm = ch->ct->GetCells();

	int freed = ch->recorded_size;

	if( ch->changed ) {
		// <save cell data>
		Job *job = JobManager::CreateJob();
		JobManager::SetupJob_Save( job, ch );
		job->callback = &CellPool::OnSaveComplete;
		job->userdata_ptr = ch;
		engine->GetJobs()->Push( job );
		
			
	} else {
		cm->UpgradeS( ch->shuffled_index );
		cm->SetUnknownCellStateS( ch->shuffled_index, CELLSTATE_DORMANT );
		cm->CompleteS( ch->shuffled_index );

		Memory::FreeMem( ch );
	}

	return freed;
}

//-------------------------------------------------------------------------------------------------
int CellPool::FreeCell() {
	
	Cell *ch = PopFront();
	if( !ch ) return 0;

	// dont delete cells which have their compressed data cached
	if( ch->codec_cache ) {
		Push( ch );
		return 0;
	}

	CellTree *cm = ch->ct->GetCells();
	
	{
		boost::unique_lock<boost::mutex> lock(*cm->GetMutex());

		if( cm->IsLockedSE( ch->shuffled_index ) ) {
			Push( ch );
			return 0;
		}
		
		cm->ReserveSE( ( ch->shuffled_index ), lock );
	}

	return DoFree( ch );
}

//-------------------------------------------------------------------------------------------------
void CellPool::CleanThread() {

	boost::unique_lock<boost::mutex> lock(clean_thread_mutex);

	while( true ) {

		if( shutting_down ) break;
		if( memory_usage > memory_quota ) {
			lock.unlock();
			int freed = FreeCell();
			lock.lock();
			memory_usage -= freed;
			//do_free = true;
		} else {
			
			cvar_clean_wakey.wait(lock);
		}
		


	}
}

//-------------------------------------------------------------------------------------------------
}
