//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//
#pragma once

#include "world/JobManager.h"
#include "world/LockTable.h"
#include "world/CommandQueue.h"
#include "world/CellTree.h"
#include "world/WorldParameters.h"
#include "world/CubeKernel.h"
#include "util/LockList.h"
#include "world/CellShadow.h"
#include "util/EventWaiter.h"

namespace World {

struct Cell;
class Engine;
	
//------------------------------------------------------------------------------------------------------------------------
struct copybox_parameters {
	int x1, y1, z1;
	int x2, y2, z2;
	int width, height, length;
	boost::uint16_t *buffer;
};


//------------------------------------------------------------------------------------------------------------------------
// Context
// holds data for a planet or instance
class Context {

	Util::LockList file_lock_table;
	
	// command queues
	// commands is from the user
	// command_execute_list is when the command is ready to be executed
	//
	CommandQueue commands;
	CommandQueue command_execute_list;

	Util::EventWaiter ew_dispatcher;

	// active command list
	// these are commands waiting to be executed and commands in the execution process
	// this list utilizes the bounding box members of each command to manage which
	// is executed before others
	//
	Command *first_active_command;
	Command *last_active_command;
	volatile int active_commands_total;
	int active_commands_quota;
	boost::mutex active_command_mutex;
	
	boost::thread command_dispatcher_thread;

	boost::thread *command_execution_threads;
	int num_command_execution_threads;
	volatile unsigned int shutting_down_command_execution;

	boost::uint64_t command_counter;

	// create a new job associated with this context
	//Job *NewJob();

	// create a generation job and push it
	//void PushGenerationJob( int world_index );

	// create a disk loading job and push it
	//void PushDiskReadJob( int world_index );

	void PushCreationJob( boost::uint64_t world_index, int unknown_state );

	// <command processing functions>
	void ProcessCommand( Command * );

	void ProcessCommandExplosion( Command *c );
	void ProcessCommandClearBox( Command *c );
	void ProcessCommandClearSingle( Command *c );
	void ProcessCommandSetBox( Command *c );
	void ProcessCommandSetSingle( Command *c );
	void ProcessCommandPaint( Command *c );

	static void CopyBoxChunkData( Context &ct, CellShadow *shadow, int x, int y, int z, copybox_parameters &p );
	static void SetBoxChunkData( Context &ct, CellShadow *shadow, int x, int y, int z, copybox_parameters &p );
	static void ClearBoxChunkData( Context &ct, CellShadow *shadow, int x, int y, int z, copybox_parameters &p );
	static void DamageBoxChunkData( Context &ct, CellShadow *shadow, int x, int y, int z, copybox_parameters &p );
	
	// manipulation utility functions

	void InternalCopyBox( int x1, int y1, int z1, int x2, int y2, int z2, boost::uint16_t *buffer );
	void InternalSetBox( CellShadow &shadow, int x1, int y1, int z1, int x2, int y2, int z2, boost::uint16_t *buffer );
	void InternalClearBox( CellShadow &shadow, int x1, int y1, int z1, int x2, int y2, int z2 );
	void InternalClearBoxMapped( CellShadow &shadow, int x1, int y1, int z1, int x2, int y2, int z2, boost::uint16_t *buffer );
	void InternalDamageBox( CellShadow &shadow, int x1, int y1, int z1, int x2, int y2, int z2, boost::uint16_t *buffer );

	template <typename F> 
	void BoxFunction( int x1, int y1, int z1, int x2, int y2, int z2, boost::uint16_t *buffer, CellShadow *shadow, F func );

	// create a chunk for an empty entry
	//Chunk *UpgradeEmptyChunk( int x, int y, int z );

	Engine *engine;

	CubeKernel *kernel;
	CellTree *cell_tree;
	
	void CommandDispatcherThread();


	void CommandExecutionThread();
	void StartCommandExecutionThreads( int count );
	void StopCommandExecutionThreads();

	void AddActiveCommand( Command *cmd );
	void RemoveActiveCommand( Command *cmd );
	//void ComputeCommandBoundingBox( Command *cmd );

	bool CommandBoxTest( Command &a, Command &b );

	Command *NewCommand();
	void SetCommandArea( Command &, int x, int y, int z, int size, bool secondary = false );
	void SetCommandArea( Command &, int x, int y, int z, int size_x, int size_y, int size_z, bool secondary = false );

	enum {
		CUBEOP_USER_GENERIC,		// when user breaks a cube
		CUBEOP_USER_EXPLOSIVE,		// when user breaks cubes with an explosion
		
		CUBEOP_SYSTEM_LARGE_AREA,	// for system operations such as box fills/clears (does raw operation without special effects)
	};

	// functions that operate directly on cell data.
	// these functions are the lowest level of operations and handle 
	//  the normal reactions when cubes are modified (spawning materials when destroyed, killing attached entities, etc)
	//
	void CubeOpRemove( Cell &cell, int index, int mode );
	void CubeOpAdd( Cell &cell, int index, int cube, int mode );
	void CubeOpChange( Cell &cell, int index, int cube, int mode );
	void CubeOpDamage( Cell &cell, int index, int amount, int mode );
	void CubeOpDamageSet( Cell &cell, int index, int amount, int mode );
	void CubeOpPaint( Cell &cell, int index, int side, int paint, int mode );
	void CubeOpDecal( Cell &cell, int index, int side, int decal, int mode );


public:
	WorldParameters map;

	const char *datapath; // path to directory with region files

	// dimensions
	int world_width,world_depth,world_length;

	int sea_level; // copied from map setting
	
	//-----------------------------------------------------------------------------------
	// initialize with map [file] and region data [directory] path
	//
	Context( const char *mapfile, const char *directory_path, int num_cmdex_threads );

	~Context();

	//
	void GetRegionAddress( boost::uint64_t world_index, boost::uint64_t *region_index, int *index_within_region );
	//void GetRegionAddress( boost::uint64_t world_index, int *region_index, int *index_within_region );
	void GetRegionFilename( boost::uint64_t world_index, std::string &output );
	
	//void GetChunkFilename( int index, std::string &output );

	CellTree *GetCells();
	Engine *GetEngine();

	// returns an actual chunk address
	// 0 if the chunk is in a non-ready state
	//Chunk *GetChunk( int x, int y, int z );
	//Chunk *GetChunk( int world_index );

	// todo: sealevel in general, where does it originate
	float GetSeaLevel() const;  

	// Refresh chunk pointer
	// refreshing bumps the chunk in the allocation queue as to postpone the allocation process targetting it
	// TODO: refreshig part of chunk locking
	//void RefreshChunk( int world_index );

	// read dimension functions
	int GetWidth() const;
	int GetDepth() const;
	int GetLength() const;
	
	// returns true if x,y,z >= 0 and x,y,z < width,height,length
	bool CoordsInChunkRange( int x, int y, int z );

	// returns true if x,y,z >= 0 and x,y,z < width*16,height*16,length*16
	bool CoordsInCubeRange( int x, int y, int z );

	// reads cube index data (if the chunk is actively loaded)
	// returns -1 on "chunk not loaded"
	// 0 if out of world range
	int GetCube( int x, int y, int z );

	// loads a chunk from disk or generates it
	// returns true if the chunk is already in a known state
	bool Request( boost::uint64_t world_index );
	bool Request( int x, int y, int z );

	// request chunks that intersect with the specified box area
	// (measured in cube units)
	// returns true if all of the chunks are loaded already
	bool RequestCubeBox( int x1, int y1, int z1, int x2, int y2, int z2 );

	// tests if an area is loaded
	// measured in cube units, any chunk that intersects the
	//  area must be loaded for the test to pass
	bool IsAreaLoaded( int x1, int y1, int z1, int x2, int y2, int z2 );

	// blocking request functions
	// blocks until the area can be read
	//
	// ideally you will already have a lock on the area to avoid it from being lost right after the request
	//
	// WaitEach means the function checks each chunk individually in a sequence
	// and not alltogether, meaning it may not be intact when the function completes
	// if a lock isn't on the entire area
	//
	void RequestWait( boost::uint64_t world_index );
	void RequestWait( int x, int y, int z );
	void RequestCubeBoxWaitEach( int x1, int y1, int z1, int x2, int y2, int z2 );
	void RequestCubeBoxWaitEach( int *pos1, int *pos2 );

	// create cell shadows
	// these functions must be used with reserved locks
	//
	void Shadow( boost::uint64_t world_index );
	void Shadow( int x, int y, int z );
	void ShadowCubeBox( int x1, int y1, int z1, int x2, int y2, int z2 );

	
	// test for solid cube
	bool Collision( int x, int y, int z ); // todo: const?
	bool Collision3f( cml::vector3f &pos );
	bool Collision3d( cml::vector3d &pos );

	//--------------------------------------------------------------------------------------
	// modification functions:
	// all modifications are buffered to ensure they are performed in the right order

	// destroy a set of cubes with origin+explosion power
	void DestroyPowered( int x, int y, int z, int power );

	// set and delete functions for single cubes
	void SetCube( int x, int y, int z, int index );
	void ClearCube( int x, int y, int z );

	// CUBOID! functions
	void SetBox( int x, int y, int z, int width, int height, int length, boost::uint16_t *data );
	void ClearBox( int x, int y, int z, int width, int height, int length );

	// paint a cube side
	void SetCubePaint( int x, int y, int z, int side, int paint );

	// routine function // todo; have a nice "update" function
	//void ProcessCommands();

	//void ExecuteCommand(); // ?
	//void FlushCommands();

	// picking function
	// tests the line segment for collisions
	// returns TRUE if collided and fills the return values:
	// intersection - the point at which the collision occurred
	// target - the block that the ray picked
	// normal - the surface normal of the face that was picked, (see enum below)
	bool Pick( const cml::vector3d &start, const cml::vector3d &end, cml::vector3d *intersection, cml::vector3i *target, int *normal );

	enum {
		NORMAL_FORWARD,	// +Z
		NORMAL_BACK,	// -Z
		NORMAL_RIGHT,	// +X
		NORMAL_LEFT,	// -X
		NORMAL_UP,		// +Y
		NORMAL_DOWN		// -Y
	};

	// use when doing io on this context's files
	void AcquireFileLock( boost::uint64_t region_index );
	void ReleaseFileLock( boost::uint64_t region_index );
};


//------------------------------------------------------------------------------------------------------------------------
typedef void (World::Context::*box_chunk_function)( int, int, int, copybox_parameters & );

}
