//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//

#pragma once

#include <boost/cstdint.hpp>

namespace World {

enum {
	JOB_MANAGER_DEFAULT_WORK_THREADS = 4
};

class Context;

struct t_Job;

typedef void (*JobCallback)( struct t_Job *job );

//-------------------------------------------------------------------------------------------------
// job instance for worker threads (ie chunk request/generation)
//
typedef struct t_Job {
	int type;					// type of job
	Context *ct;				// context to operate on
	boost::uint64_t world_index;// index of cell that this operates on (if applicable)
	int data[4];				// extra data

	int userdata[4];			// generic space to store data
	void *userdata_ptr;			// and a pointer to extend this space

	JobCallback callback;

	struct t_Job * next;	// linked list pointer
} Job;

//-------------------------------------------------------------------------------------------------
enum {
	JOB_INVALID,
	// job: request chunk, client side only
	// requests chunk data from the server
	JOB_REQUEST,

	// job: generate chunk
	// performs terrain generation to fill a chunk slot
	JOB_GENERATE,

	// job: save chunk to disk
	JOB_SAVE,

	// job: load a chunk from disk
	JOB_LOAD,

	// generate and/or load
	// data1 = unknown chunk state
	//		   CHUNKSTATE_UNKNOWN = unknown if there is disk data or not (must check filesystem)
	//         CHUNKSTATE_DORMANT = there is no disk data
	//         CHUNKSTATE_DISK = there is saved data on disk to overlay on the creation
	JOB_CREATE,
};

//------------------------------------------------------------------------------------------------------------------------
// class for managing JOBs
// jobs are global operations which can be performed in any order (thread pooled)

class JobManager {

private:
	Job *first;
	Job *last;

	//volatile int work_threads_free;
	volatile unsigned int total_jobs_queued;

	//boost::mutex lock;

	boost::thread *work_threads;
	
	// condition variables for thread sleeping
	// 0 = main thread, 1+ = work threads
	boost::condition_variable work_added;
	boost::mutex work_list_mutex;
	//boost::condition_variable work_ready[JOB_MANAGER_WORK_THREADS+1];
	//boost::mutex work_mutex[JOB_MANAGER_WORK_THREADS+1];
	

	// jobs for work threads to complete
	//job *work_jobs[JOB_MANAGER_WORK_THREADS];

	volatile bool stopping;
	volatile unsigned int stop_threads;

	int num_work_threads;

	//void SendWorkSignal( int slot );
	//void SleepUntilReady( int slot );

	//void MainSleep();
	Job * GetWork();
	
	//void MainThread();
	void WorkThread( int slot );

	//int GetFreeWorkSlot();
	//int GetFreeWorkSlots();

	Job * PopE();

	void ExecuteJob( Job *, bool decrement = true );
	
public:
	
	enum {
		DEFAULT_WORK_THREADS = 4
	};

	JobManager( int num_workers );
	~JobManager();

	void Push( Job *job );

	//void Flush();

	int GetPendingJobs() const;
	
	static Job * CreateJob( bool zero = true );
	static void SetupJob_Save( Job *j, Cell *cell );
};

}