//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//

#include "stdafx.h"


namespace World {

//-------------------------------------------------------------------------------------------------
JobManager::JobManager( int num_workers ) {
	
	stopping = false;
	stop_threads = false;
	first = last = 0;
	num_work_threads = num_workers;

	work_threads = new boost::thread[num_workers];

	// allocate work threads
	for( int i = 0; i < num_workers; i++ ) {
		work_threads[i] = boost::thread( boost::bind( &JobManager::WorkThread, this, i ) );
	}
}

//-------------------------------------------------------------------------------------------------
JobManager::~JobManager() {
	{
		boost::lock_guard<boost::mutex> guard(work_list_mutex);
		stop_threads = true;
	}
	
	work_added.notify_all();

	for( int i = 0; i < num_work_threads; i++ ) {
		work_threads[i].join();
	}

	delete[] work_threads;
}

//-------------------------------------------------------------------------------------------------
void JobManager::ExecuteJob( Job *j, bool decrement ) {

	assert( j->type != JOB_INVALID );

	switch( j->type ) {
	case JOB_REQUEST:
		// todo
		break;
	//case JOB_GENERATE: {
	//	Chunk *ch = Generation::Create( j->ct, j->index );
	//	// todo...
	//	} break;
	case JOB_SAVE:
		// todo
		break;
	//case JOB_LOAD:
	//	// todo
	//	break;
	case JOB_CREATE:
		//2do
		break;
	}

	if( j->callback ) {
		j->callback( j );
	}
	
	Memory::FreeMem(j);

	if( decrement ) {
		InterlockedDecrement(&total_jobs_queued); 
	}
}
/*
//-------------------------------------------------------------------------------------------------
void JobManager::Start() {
	stopping = false;
	
	threads[0] = boost::thread( boost::bind( &JobManager::MainThread, this ) );

//	work_threads_free = JOB_MANAGER_WORK_THREADS;
	for( int i = 0; i < JOB_MANAGER_WORK_THREADS; i++ ) {
		work_jobs[i] = 0;
		threads[1+i] = boost::thread( boost::bind( &JobManager::WorkThread, this, i ) );
		
	}
	
	enabled = true;
}*/
/*
//-------------------------------------------------------------------------------------------------
void JobManager::Stop() {
	
	stopping = true;

	// todo wait for threads to stop


	enabled = false;
}*/
/*
//-------------------------------------------------------------------------------------------------
void JobManager::SendWorkSignal( int slot ) {
	boost::lock_guard<boost::mutex> lock(work_mutex[1+slot]);
	work_ready[slot].notify_one();
}*/
/*
//-------------------------------------------------------------------------------------------------
void JobManager::MainSleep() {
	if( !first && !stopping ) {
		boost::unique_lock<boost::mutex> lock(work_mutex[0]);
		while( !first && !stopping ) {
			work_ready[0].wait(lock);
		}
	}
}*/
/*
//-------------------------------------------------------------------------------------------------
void JobManager::WaitForWork( int slot ) {

	boost::unique_lock<boost::mutex> work_list_mutex;

	if( !work_jobs[slot] && !stop_threads ) {
		boost::unique_lock<boost::mutex> lock(work_mutex[1+slot]);
		while( !work_jobs[slot] && !stop_threads ) {
			work_ready[1+slot].wait(lock);
		}
	}
}*/

//-------------------------------------------------------------------------------------------------
void JobManager::Push( Job *j ) {
//	if( !enabled ) return;

	InterlockedIncrement(&total_jobs_queued);
	j->next = 0;

	{
		boost::lock_guard<boost::mutex> guard(work_list_mutex);

		if( last ) {
			last->next = j;
			last = j;
		} else {
			first = last = j;
		}
	}

	work_added.notify_one();
	//SendWorkSignal(0);
}

//-------------------------------------------------------------------------------------------------
Job * JobManager::PopE() {
	
	if( first ) {
		Job *result = first;
		first = first->next;
		if( !first ) {
			first = last = 0;
		}
		
		return result;
	}
	return 0;
}
/*
//-------------------------------------------------------------------------------------------------
void JobManager::MainThread() {
	
	while(1) {
		MainSleep();

		// look for free work slot
		int slot = GetFreeWorkSlot();
		if( slot < 0 ) {
			continue;
		}

		job *j = Pop();
		if( !j ) {
			if( stopping ) {
				if( GetFreeWorkSlots() == JOB_MANAGER_WORK_THREADS ) {
					stop_threads = true;

					// stop all work threads
					for( int i = 0; i < JOB_MANAGER_WORK_THREADS; i++ ) {
						SendWorkSignal(1+i);
					}
					break;
				}
			}
		} else {
			work_jobs[slot] = j;
			work_ready[1+slot].notify_one();
		}
	}
}
*/
//-------------------------------------------------------------------------------------------------
Job * JobManager::GetWork() {
	boost::unique_lock<boost::mutex> lock(work_list_mutex);
	Job *job = 0;
	while(true) {
		job = PopE();
		if( job || stop_threads ) {
			break;
		}
		work_added.wait(lock);
	}
	return job;
}

//-------------------------------------------------------------------------------------------------
void JobManager::WorkThread( int slot ) {

	while(1) {
		Job *job = GetWork();

		if( job ) {
			ExecuteJob( job );
		}

		if( stop_threads ) {
			break;
		}
	}
	
}
/*
//-------------------------------------------------------------------------------------------------
int JobManager::GetFreeWorkSlot() {
	for( int i = 0; i < JOB_MANAGER_WORK_THREADS; i++ ) {
		if( work_jobs[i] == 0 ) {
			return i;
		}
	}
	return -1;
}*/
/*
//-------------------------------------------------------------------------------------------------
int JobManager::GetFreeWorkSlots() {
	int count = 0;
	for( int i = 0; i < JOB_MANAGER_WORK_THREADS; i++ ) {
		count++;
	}
	return -1;
}
*/
//-------------------------------------------------------------------------------------------------
int JobManager::GetPendingJobs() const {
	return total_jobs_queued;
}

//-------------------------------------------------------------------------------------------------
Job *JobManager::CreateJob( bool zero ) {
	Job *j = (Job*)Memory::AllocMem( sizeof(Job) );
	if( zero ) {
		memset( j, 0, sizeof(j) );
	}
	return j;
}

//-------------------------------------------------------------------------------------------------
void JobManager::SetupJob_Save( Job *j, Cell *ch ) {
	j->type = JOB_SAVE;
	j->world_index = ch->world_index;
	j->ct = ch->ct;
}

}

