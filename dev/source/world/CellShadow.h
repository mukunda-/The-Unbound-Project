//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//

#pragma once

// cell shadow creates buffers for operations which change a cell
// cell shadows must only be used when the thread owns a reserved lock on a cell
//

#include "world/Cell.h"

namespace World {

//-------------------------------------------------------------------------------------------------
// creates a shadow buffer for a single cell
//
class CellShadowSingle {
	Context *context;
	Cell *cell;
	int pos[3];

	bool applied;

	bool CopyCell();

public:
	CellShadowSingle( Context &, int x, int y, int z );
	CellShadowSingle( Context &, boost::uint64_t index );
	~CellShadowSingle();

	// return pointer to shadow buffer
	//
	Cell *GetCell();

	// apply changes to cell
	// this must only be done after the reserve lock is upgraded to exclusive
	//
	void Apply();

	// after Apply, this instance becomes invalid and will assert
	// if any more calls are made
};

//-------------------------------------------------------------------------------------------------
// creates a shadow buffer for an area
//
class CellShadow {
	Context *context;
	Cell **cells;
	int pos[3];
	int size[3];

	bool applied;

	bool CopyCell( int x, int y, int z);

public:
	CellShadow( Context &, int x1, int y1, int z1, int x2, int y2, int z2 );
	CellShadow( Context &, const int *pos1, const int *pos2 );
	~CellShadow();

	// get cell at location
	// the cell must exist within the shadowed area
	// or 0 will be returned
	//
	Cell * GetCell( int x, int y, int z );
	Cell * GetCell( boost::uint64_t index );

	// apply changes
	// this must only be done after the reserve lock is upgraded to exclusive
	//
	void Apply();

	// after Apply, this instance becomes invalid and will assert
	// if any more calls are made
};


}
