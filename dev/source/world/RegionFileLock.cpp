//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//

#include "stdafx.h"

namespace World {

//-------------------------------------------------------------------------------------------------
RegionFileLock::RegionFileLock( Context *ct, int p_region_index ) {
	region_index = p_region_index;
	context = ct;
	if( !context ) return;
	context->AcquireFileLock( region_index );
}

//-------------------------------------------------------------------------------------------------
RegionFileLock::~RegionFileLock() {
	if( context && region_index >= 0 ) {
		context->ReleaseFileLock( region_index );
	}
}

}
