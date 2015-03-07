//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//

#pragma once
  
static FILE * fopen2( const char *filename, const char *mode ) {
#if _MSC_VER
	FILE *f=0;
	fopen_s( &f, filename, mode );
	return f;
#else
	if( filename == 0 ) return 0;
	return fopen( filename, mode );
#endif
}
