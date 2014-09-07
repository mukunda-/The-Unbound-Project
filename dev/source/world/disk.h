//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//

#pragma once


namespace World {
namespace Disk {

	/*
namespace Codec {

	// pack/unpack function parameters:
	// input = source data
	// output = output destination
	// input_size = size of input data (bytes or words)
	// max_output_size = allocated size of output buffer (bytes (RLE1) or words (RLE2))

	// error = pointer to error boolean
	//   set to true if an error occured with the data (ie output data overflow)
	// size of unpacked/packed data passed as return value

	int PackRLE1( boost::uint8_t *input, boost::uint8_t *output, int input_size, bool *error );
	int PackRLE2( boost::uint16_t *input, boost::uint8_t *output, int input_size, bool *error );
	int UnpackRLE1( boost::uint8_t *input, boost::uint8_t *output, int input_size, int max_output_size, bool *error );
	int UnpackRLE2( boost::uint8_t *input, boost::uint16_t *output, int input_size, int max_output_size, bool *error );
}*/

enum {
	REGION_SIZE = 4
};

struct ItemHeader {
	ItemHeader *next;	// linked list data
	int size;			// size of the entity data (bytes) including this header
	int	type;			// type of entity
	int local_index;	// positioning within the chunk
	// <entity specific data here>
};

struct EntityHeader {
	EntityHeader *next;
	int size;
	int type;
};

typedef ItemHeader* (*ProcessItemsFunction)( ItemHeader *item, void *userdata );
typedef EntityHeader* (*ProcessEntitiesFunction)( EntityHeader *ent, void *userdata );

//------------------------------------------------------------------------------------------------------------------------------
// parameters for the following functions:
//
// ct: context to perform operation on
// world_index: index of chunk in world
// 

//------------------------------------------------------------------------------------------------------------------------------
// push a new item into a chunk
//
// data: chunk entity data
//
void PushItem( Context *ct, int index, ItemHeader *data );

//------------------------------------------------------------------------------------------------------------------------------
// push a list of items into a chunk
// 
void PushItems( Context *ct, int index, ItemHeader *first, ItemHeader *last );

//------------------------------------------------------------------------------------------------------------------------------
// pop all items from a chunk
//
// returns list in first,last
//
void PopItems( Context *ct, int index, ItemHeader **first, ItemHeader **last );

//------------------------------------------------------------------------------------------------------------------------------
// runs a function on items in a chunk
//
// local: if -1, processes all items in this chunk, otherwise only processes items associated with this cube index
// data: userdata to be passed to the callback
// func: callback function to operate on the items
//
void ProcessItems( Context *ct, int index, int local, void* data, ProcessItemsFunction func );


//------------------------------------------------------------------------------------------------------------------------------
// push an entity into a chunk
//
void PushEntity( Context *ct, int index, EntityHeader * );

//------------------------------------------------------------------------------------------------------------------------------
// push a list of entities into a chunk
//
void PushEntities( Context *ct, int index, EntityHeader *first, EntityHeader *last );

//------------------------------------------------------------------------------------------------------------------------------
// pop all entities from a chunk
//
void PopEntities( Context *ct, int index, EntityHeader **first, EntityHeader **last );

//------------------------------------------------------------------------------------------------------------------------------
// process entities in a chunk
//
// data: userdata passed to callback
//
void ProcessEntities( Context *ct, int index, void *data, ProcessEntitiesFunction func );

//------------------------------------------------------------------------------------------------------------------------------
// read chunk data from disk
//
Cell * ReadCellData( Context *ct, boost::uint64_t world_index );

//------------------------------------------------------------------------------------------------------------------------------
// save chunk data to disk
//
void SaveCellData( Context *ct, boost::uint64_t world_index, Cell *data );

/*
struct RegionChunk {
	boost::uint8_t *cube_data;		// compressed cube data
	boost::uint8_t *paint_data;		// compressed paint data
	boost::uint8_t *extra_data;		// compressed extra cube data

	ChunkEntityHeader *entities_first;	// list of entities stored in this chunk
	ChunkEntityHeader *entities_last;
	
};

struct Region {
	RegionChunk *chunks[REGION_SIZE*REGION_SIZE*REGION_SIZE];
};*/

}

}
