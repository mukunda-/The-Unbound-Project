//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

#include "stdafx.h"

#if 0

//-------------------------------------------------------------------------------------------------
DString::DString() {
	data = 0;
	length = 0;
}

//-------------------------------------------------------------------------------------------------
DString::~DString() {
	Memory::FreeMem( data );
}

//-------------------------------------------------------------------------------------------------
int DString::Length() {
	if( length < 0 ) CacheLength();
	return length;
}

//-------------------------------------------------------------------------------------------------
bool DString::Empty() {
	if( !data ) return true;
	if( data[0] == 0 ) return true;
}

//-------------------------------------------------------------------------------------------------
void DString::CacheLength() {
	if( !data ) {
		length = 0;
		return;
	}
	for( length = 0; length < Memory::SizeOf( data ); length++ ) {
		if( data[length] == 0 ) return;
	}
}

#endif
