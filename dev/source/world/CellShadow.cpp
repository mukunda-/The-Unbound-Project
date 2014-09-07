//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//

#include "stdafx.h"

namespace World {

//-------------------------------------------------------------------------------------------------
bool CellShadowSingle::CopyCell() {
	Cell *address;
	int state;
	CellTree *cm = context->GetCells();
	CellCodec *codec = context->GetEngine()->GetCodec();

	
	cm->GetCellState( pos[0], pos[1], pos[2], &state, &address );
	if( !address->codec_cache ) {
		codec->Decode( context, PackCoords( pos[0], pos[1], pos[2] ) );
	}

	if( !(state & CELLSTATE_READY) ) {
		return false;
	}

	cell = (Cell*)Memory::AllocMem( sizeof(Cell) );
	

	if( ((state & CELLSTATE_READY) && (state & CELLSTATE_EMPTY)) || (!(state & CELLSTATE_READY)) ) {

		// empty
		InitializeCellHeader( cell );
		for( int i = 0; i< 16*16*16; i++ ) {
			cell->cubes[i] = 0;
		}
		cell->empty = true;
	} else {
		for( int i = 0; i< 16*16*16; i++ ) {
			cell->cubes[i] = address->cubes[i];
		}
		cell->world_index = PackCoords( pos[0], pos[1], pos[2] );
		cell->ct = context;
		cell->shuffled_index = cm->ShuffleCoords( pos[0], pos[1], pos[2] );
		cell->render_flags = 0;
		cell->owner = address->owner;
		cell->group = address->group;
		cell->changed = address->changed;
		cell->empty = address->empty;
		
		cell->codec_cache = ( CellCompCache* )Memory::AllocMem( sizeof( CellCompCache ) );
		memcpy( (void*)cell->codec_cache, (void*)address->codec_cache, sizeof( CellCompCache ) );
		//cell->compressed = address->compressed;

	}
	return true;
}

//-------------------------------------------------------------------------------------------------
CellShadowSingle::CellShadowSingle( Context &ct, boost::uint64_t index ) {
	applied = false;
	context = &ct;
	UnpackCoords( index, pos[0], pos[1], pos[2] );
	assert( CopyCell() );
}

//-------------------------------------------------------------------------------------------------
CellShadowSingle::CellShadowSingle( Context &ct, int x, int y, int z ) {
	applied = false;
	context = &ct;
	pos[0] = x;
	pos[1] = y;
	pos[2] = z;
	assert( CopyCell() );
}

CellShadowSingle::~CellShadowSingle() {
	assert( !applied ); // poopsauce
}

//-------------------------------------------------------------------------------------------------
Cell *CellShadowSingle::GetCell() {
	assert( !applied );
	return cell;
}

//-------------------------------------------------------------------------------------------------
void CellShadowSingle::Apply() {
	assert( !applied );
	applied = true;
	CellTree *t = context->GetCells();

	Cell *original;
	int ostate;

	t->GetCellStateS( cell->shuffled_index, &ostate, &original );
	
	CellPool *pool = context->GetEngine()->GetCellPool();
	if( (ostate & CELLSTATE_READY) && (!(ostate&CELLSTATE_EMPTY)) ) {
		pool->Delete( original );
	}

	t->SetCellStateS( cell->shuffled_index, (CELLSTATE_READY|CELLSTATE_RESERVED|CELLSTATE_LOCKED), cell );
		
	pool->Push( cell );
}

//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
CellShadow::CellShadow( Context &ct, int x1, int y1, int z1, int x2, int y2, int z2 ) {
	int pos1[3], pos2[3];
	pos1[0] = x1;
	pos1[1] = y1;
	pos1[2] = z1;
	pos2[0] = x2;
	pos2[1] = y2;
	pos2[2] = z2;
	CellShadow( ct, pos1, pos2 );
}

//-------------------------------------------------------------------------------------------------
CellShadow::CellShadow( Context &ct, const int *pos1, const int *pos2 ) {
	for( int i = 0; i < 3; i++ ) {
		pos[i] = pos1[i];
		size[i] = pos2[i] - pos1[i] + 1;
		assert( size[i] <= 0 );
	}

	cells = (Cell**)Memory::AllocMem( size[0] * size[1] * size[2] * sizeof(Cell*) );
	

	context = &ct;

	applied = false;
	for( int y = 0; y < size[1]; y++ ) {
		for( int z = 0; z < size[2]; z++ ) {
			for( int x = 0; x < size[0]; x++ ) {
				assert( CopyCell( x, y, z ) );
			}
		}
	}
	
}

//-------------------------------------------------------------------------------------------------
CellShadow::~CellShadow() {
	assert( !applied ); // poopsauce?
	
}

//-------------------------------------------------------------------------------------------------
bool CellShadow::CopyCell( int x, int y, int z ) {
	int index = x + y * size[0]*size[1] + z * size[0];

	Cell *address;
	int state;
	CellTree *cm = context->GetCells();
	CellCodec *codec = context->GetEngine()->GetCodec();

	
	cm->GetCellState( pos[0]+x, pos[1]+y, pos[2]+z, &state, &address );
	if( !address->codec_cache ) {
		codec->Decode( context, PackCoords( pos[0]+x, pos[1]+y, pos[2]+z ) );
	}


	if( !(state & CELLSTATE_READY) ) {
		return false;
	}

	Cell *cell = cells[index] = (Cell*)Memory::AllocMem( sizeof(Cell) );

	if( ((state & CELLSTATE_READY) && (state & CELLSTATE_EMPTY)) || (!(state & CELLSTATE_READY)) ) {

		// empty
		InitializeCellHeader( cell );
		for( int i = 0; i< 16*16*16; i++ ) {
			cell->cubes[i] = 0;
		}
		cell->empty = true;
	} else {
		for( int i = 0; i< 16*16*16; i++ ) {
			cell->cubes[i] = address->cubes[i];
		}
		cell->world_index = PackCoords( pos[0], pos[1], pos[2] );
		cell->ct = context;
		cell->shuffled_index = cm->ShuffleCoords( pos[0], pos[1], pos[2] );
		cell->render_flags = 0;
		cell->owner = address->owner;
		cell->group = address->group;
		cell->changed = address->changed;
		cell->empty = address->empty;
		
		cell->codec_cache = ( CellCompCache* )Memory::AllocMem( sizeof( CellCompCache ) );
		memcpy( (void*)cell->codec_cache, (void*)address->codec_cache, sizeof( CellCompCache ) );
		//cell->compressed = address->compressed;

	}
	return true;
}

//-------------------------------------------------------------------------------------------------
Cell *CellShadow::GetCell( int x, int y, int z ) {
	assert( !applied );
	if( x < 0 || y < 0 || z < 0 || x >= size[0] || y >= size[1] || z >= size[2] ) return 0;
	int index = x + y * size[0]*size[1] + z * size[0];
	return cells[index];
}

//-------------------------------------------------------------------------------------------------
Cell *CellShadow::GetCell( boost::uint64_t index ) {
	assert( !applied );
	int x, y, z;
	UnpackCoords( index, x, y, z );
	return GetCell( x, y, z );
}

//-------------------------------------------------------------------------------------------------
void CellShadow::Apply() {
	assert( !applied );
	applied = true;

	CellTree *t = context->GetCells();
	
	Cell *original;
	int ostate;

	CellPool *pool = context->GetEngine()->GetCellPool();

	int x,y,z;
	for( y = 0; y < size[1]; y++ ) {
		for( z = 0; z < size[2]; z++ ) {
			for( x = 0; x < size[0]; x++ ) {
				int index = x + y * size[0]*size[1] + z * size[0];
				t->GetCellStateS( cells[index]->shuffled_index, &ostate, &original );
				if( (ostate & CELLSTATE_READY) && (!(ostate&CELLSTATE_EMPTY)) ) {
					pool->Delete( original );
				}

				t->SetCellStateS( cells[index]->shuffled_index, (CELLSTATE_READY|CELLSTATE_RESERVED|CELLSTATE_LOCKED), cells[index] );
		
				pool->Push( cells[index] );
			}
		}
	}


	Memory::FreeMem( cells );
	cells = 0;
	
	
}

//-------------------------------------------------------------------------------------------------
}
