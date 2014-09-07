//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//

//-------------------------------------------------------------------------------------------------
#pragma once

//-------------------------------------------------------------------------------------------------
#include <boost/thread.hpp>
#include "world/Cell.h"

//-------------------------------------------------------------------------------------------------
namespace World {

typedef struct t_CCLocation {

	Context *context;
	boost::uint64_t shuffled_index;
	
	struct t_CCLocation *next;

} CCLocation;

//-------------------------------------------------------------------------------------------------
class CellCodec {
	
	//-----------------------------------------------------------------------------------
	CCLocation *cd_first;
	CCLocation *cd_last;

	boost::mutex mut;
	boost::thread memory_thread;

	boost::condition_variable cvar_cell_decoded;

	//-----------------------------------------------------------------------------------
	int memory_used;		// number of caches that are allocated
	int memory_quota;		// quota of caches that can be allocated
	boost::condition_variable cvar_memory_quota_exceeded;
	bool shutting_down;

	//-----------------------------------------------------------------------------------
	// thread that deletes codec data that is old and not being used
	//
	void MemoryThread();

	//-----------------------------------------------------------------------------------
	// decode a cell function
	//
	void DoDecode( Cell *cell );

public:

	//-----------------------------------------------------------------------------------
	CellCodec( int quota );
	~CellCodec();

	//-----------------------------------------------------------------------------------
	// do not call for chunks that are unlocked
	// use only with a shared or higher lock on a cell
	//
	void Decode( Context *ct, boost::uint64_t index );

	//-----------------------------------------------------------------------------------
	// packing functions
	// output size for packing functions must be large enough to hold all of the compressed data
	//  (*todo: describe)
	// 
	// input: source data (should be 32bit aligned) (64bit for rle2?)
	// input_size: specified in bytes, must must be multiple of 4 for RLE1, 8 for RLE2
	// output: packed/unpacked destination
	// expected_output_size: specified in bytes, output data size must equal this or it will flag it as an error
	// *error: bool to receive error status (underflow or overflow)
	//
	static int PackRLE1( boost::uint8_t *input, int input_size, boost::uint8_t *output );
	static int PackRLE2( boost::uint16_t *input, int input_size, boost::uint8_t *output );
	static int UnpackRLE1( boost::uint8_t *input, int input_size, boost::uint8_t *output, int expected_output_size, bool *error );
	static int UnpackRLE2( boost::uint8_t *input, int input_size, boost::uint16_t *output, int expected_output_size, bool *error );
};

//-------------------------------------------------------------------------------------------------
}
