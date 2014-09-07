//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//

#include "stdafx.h"

//-------------------------------------------------------------------------------------------------
namespace World {

enum {
	STATE_UNCACHED,
	STATE_DECODING,
	STATE_READY
};

//-------------------------------------------------------------------------------------------------
CellCodec::CellCodec( int quota ) {
	cd_first = cd_last = 0;
	
	boost::lock_guard<boost::mutex> lock(mut);
	{
		memory_used = 0;
		memory_quota = quota;
		
		shutting_down = false;
	}
	memory_thread = boost::thread( boost::bind( &CellCodec::MemoryThread, this ) );
}

//-------------------------------------------------------------------------------------------------
CellCodec::~CellCodec() {
	{
		boost::lock_guard<boost::mutex> lock(mut);
		shutting_down = true;
	}
	cvar_memory_quota_exceeded.notify_all();

}

//-------------------------------------------------------------------------------------------------
void CellCodec::MemoryThread() {
	boost::unique_lock<boost::mutex> lock(mut);
	while( true ) {
		int amount_to_free = 0;
		
		
		{
			
			while( true ) {
				amount_to_free = memory_used - memory_quota;
				if( amount_to_free > 0 || shutting_down ) break;
				cvar_memory_quota_exceeded.wait(lock);
			}
		}

		{
			CCLocation *copy_first, *copy_last;

			if( amount_to_free > 0 ) {
				// unlink list
				copy_first = cd_first;
				copy_last = cd_last;
				cd_first = cd_last = 0;

				// unlock

			}
			while( true ) {

			}
		}
	}
}

//-------------------------------------------------------------------------------------------------
void CellCodec::DoDecode( Cell *cell ) {

	cell->codec_cache = (CellCompCache*)Memory::AllocMem( sizeof(CellCompCache) );

	int position = 0;
	UnpackRLE1( cell->compressed + position, cell->comp_sizes[COMP_DAMAGE], cell->codec_cache->damage, CELL_DAMAGE_SIZE, 0 );
	UnpackRLE2( cell->compressed + position, cell->comp_sizes[COMP_PAINT], cell->codec_cache->paint, CELL_PAINT_SIZE, 0 );
	UnpackRLE1( cell->compressed + position, cell->comp_sizes[COMP_DECAL], cell->codec_cache->decal, CELL_DECAL_SIZE, 0 );
	{
		boost::unique_lock<boost::mutex> lock(mut);
		cell->codec_cache_state = STATE_READY;

		 // REGISTER THIS CELL
		CCLocation *ccl = (CCLocation*)Memory::AllocMem( sizeof(CCLocation) );
		ccl->shuffled_index = cell->shuffled_index;
		ccl->context = cell->ct;
		ccl->next = 0;
		if( cd_last ) {
			cd_last->next = ccl;
			cd_last = ccl;
		} else {
			cd_first = cd_last = ccl;          
		}
	}
}

//-------------------------------------------------------------------------------------------------
void CellCodec::Decode( Context *ct, boost::uint64_t index ) {
	
	CellTree *tree = ct->GetCells();
	
	Cell *cell;
	tree->GetCellState( index, 0, &cell );

	assert( cell );

	int decode = 0;

	{
		boost::unique_lock<boost::mutex> lock(mut);
		while( true ) {

			if( cell->codec_cache_state == STATE_UNCACHED ) { // uncached
				cell->codec_cache_state = STATE_DECODING;
				decode = 1;
			} else if( cell->codec_cache_state == STATE_READY ) {
				return;
			} else if( cell->codec_cache_state == STATE_DECODING ) {
				cvar_cell_decoded.wait(lock);
			}
		}
	}

	if( decode ) {
		DoDecode( cell );
	}
}

//-------------------------------------------------------------------------------------------------
int CellCodec::PackRLE1( boost::uint8_t *input, int input_size, boost::uint8_t *output ) {
	int read = 0;
	boost::uint8_t *pout = output;

	boost::uint32_t *input32 = (boost::uint32_t*)input;

	assert( !(input_size & 3) );

	int input_size32 = input_size >> 2;

	while( read < input_size32 ) {
		boost::uint32_t dword = input32[read];
		int count;
		for( count = 1; count < 256; count++ ) {
			if( read+count >= input_size32 ) break;
			if( input32[read+count] != dword ) break;

		}

		*pout++ = count-1;
		*((boost::uint32_t*)pout) = dword;
		pout += 4;

		read += count;
	} 

	return (int)(pout - output);
}

//-------------------------------------------------------------------------------------------------
int CellCodec::PackRLE2( boost::uint16_t *input, int input_size, boost::uint8_t *output ) {
	int read = 0;
	boost::uint8_t *pout = output;
	boost::uint32_t *input32 = (boost::uint32_t*)input;
	assert( !(input_size & 7) );
	int input_size32 = input_size >> 2;

	while( read < input_size32 ) {
		boost::uint32_t qword[2];
		qword[0] = input32[read];
		qword[1] = input32[read+1];
		
		int count;
		for( count = 1; count < 256; count++ ) {
			if( read+count*2 >= input_size32 ) break;
			if( input32[read+count*2] != qword[0] || input32[read+count*2+1] != qword[1] ) break;
		}

		*pout++ = count-1;
		*((boost::uint32_t*)pout) = qword[0];
		pout += 4;
		*((boost::uint32_t*)pout) = qword[1];
		pout += 4;

		read += count;
	}

	return (int)(pout - output);
}

//-------------------------------------------------------------------------------------------------
int CellCodec::UnpackRLE1( boost::uint8_t *input, int input_size, boost::uint8_t *output, int expected_output_size, bool *error ) {
	if( error ) *error = true;
	int read = 0;
	int write = 0;

	assert( !(expected_output_size & 3) );

	int remaining_dwords = expected_output_size >> 2;

	boost::uint32_t *output32 = (boost::uint32_t*)output;
	
	if( input_size % 5 ) return 0; // error - input must be multiple of 5

	while( read != input_size ) {
		int count = input[read++] + 1;
		boost::uint32_t data = *((boost::uint32_t*)(input+read));
		read += 4;
		
		if( count > remaining_dwords ) return 0; // overflow
		remaining_dwords -= count;
		
		for( ; count > 0; count-- ) {
			*output32++ = data;
		}

	}

	if( remaining_dwords ) return 0; // underflow
	if( error ) *error = false;
	return expected_output_size;
}

//-------------------------------------------------------------------------------------------------
int CellCodec::UnpackRLE2( boost::uint8_t *input, int input_size, boost::uint16_t *output, int expected_output_size, bool *error ) {
	if( error ) *error = true;
	int read = 0;
	int write = 0;

	assert( !(expected_output_size & 7) ); // error - output must be multiple of 8

	int remaining_qwords = expected_output_size >> 2;

	boost::uint32_t *output32 = (boost::uint32_t*)output;

	if( input_size % 9 ) return 0; // error - input must be multiple of 9 (underflow)

	while( read != input_size ) {
		int count = input[read++] + 1;
		boost::uint32_t qword[2];
		qword[0] = *((boost::uint32_t*)(input+read));
		qword[1] = *((boost::uint32_t*)(input+read+4));
		read += 8;

		if( count > remaining_qwords ) return 0; // error - overflow
		remaining_qwords -= count;

		for( ; count > 0; count-- ) {
			*output32++ = qword[0];
			*output32++ = qword[1];
		}
	}

	if( remaining_qwords ) return 0; // error - underflow
	if( error ) *error = false;
	return expected_output_size;
}

}
