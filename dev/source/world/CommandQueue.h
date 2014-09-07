//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//

#pragma once

namespace World {

class Context;

//-------------------------------------------------------------------------------------------------
enum {

	CMDH_BLOCK_MODS = 0x0100,

	// clear radius command
	// data: X, Y, Z, RADIUS
	//CMD_CLEAR_RADIUS = 0x0101,

	// clear explosion command
	// data: X, Y, Z, POWER
	CMDL_CLEAR_EXPLOSION	= 0x01,
	CMD_CLEAR_EXPLOSION		= CMDH_BLOCK_MODS | CMDL_CLEAR_EXPLOSION,

	// clear box command
	// data: X,Y,Z,WIDTH,HEIGHT,LENGTH
	CMDL_CLEAR_BOX			= 0x02,
	CMD_CLEAR_BOX			= CMDH_BLOCK_MODS | CMDL_CLEAR_BOX,

	// clear single cell
	// data: X, Y, Z
	CMDL_CLEAR_SINGLE		= 0x03,
	CMD_CLEAR_SINGLE		= CMDH_BLOCK_MODS | CMDL_CLEAR_SINGLE,

	// set box command
	// data: X, Y, Z, WIDTH, HEIGHT, LENGTH, DATA
	CMDL_SET_BOX			= 0x10,
	CMD_SET_BOX				= CMDH_BLOCK_MODS | CMDL_SET_BOX,

	// set single cell command
	// data: X, Y, Z, DATA
	CMDL_SET_SINGLE			= 0x11,
	CMD_SET_SINGLE			=  CMDH_BLOCK_MODS | CMDL_SET_SINGLE,
	
	// paint single side
	// data: X, Y, Z, PAINT, SIDE
	CMDL_PAINT				= 0x20,
	CMD_PAINT				= CMDH_BLOCK_MODS | CMDL_PAINT,
};
	
//------------------------------------------------------------------------------------------------------------------------
typedef struct t_BoundingBox {
	int position[3];
	int size[3];
} BoundingBox;

//------------------------------------------------------------------------------------------------------------------------
typedef struct t_CommandArea {
	BoundingBox box;
	BoundingBox box2;
	bool use_box2;
} CommandArea;

//-------------------------------------------------------------------------------------------------
// command instance for world modifications
typedef struct t_Command {
	boost::uint16_t world_index; // for applicable operations
	int data[16];			// parameters for command
	int id;					// command ID
	
	Context *ct;			// for context specific commands

	boost::uint64_t index;			// filled in and used by command dispatcher
	int executing;					// true if being executed
	volatile unsigned int finished;	// true if command is completed
	CommandArea area;				// filled in and used by command dispatcher

	struct t_Command *cq_next;		// used by command queue

	struct t_Command *ac_next;		// used by active list
	struct t_Command *ac_prev;		//
} Command;

//------------------------------------------------------------------------------------------------------------------------
// class for managing commands
//
// commands are in-order operations which modify data in a context

class CommandQueue {
	Command *first;
	Command *last;

	boost::mutex mut;
	boost::condition_variable commands_added;	

	bool enabled;

	volatile bool disable_waits;

public:
	
	CommandQueue();
	~CommandQueue();

	// retrieve current command
	Command *Get();

	// pop current command (delete it)
	Command *Pop();

	// push new command into queue
	void Push( Command * cmd );

	// called by threads that want to sleep until a command is added
	void WaitForCommand();

	// wakes up all threads sleeping in WaitForCommand and disable waiting
	void DisableWaits();
	void EnableWaits();
};

}
