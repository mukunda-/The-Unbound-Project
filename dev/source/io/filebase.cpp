//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//

#include "stdafx.h"

//-------------------------------------------------------------------------------------------------
namespace IO {

#define MAXPATH 1024

char working_directory[MAXPATH];

//-------------------------------------------------------------------------------------------------
void GetWorkingDirectory( char *path, int maxlen ) {
	strcpy_s( path, maxlen, working_directory );
}

//-------------------------------------------------------------------------------------------------
void SetWorkingDirectory( const char *path ) {
	strcpy_s<MAXPATH>( working_directory, path );
}

//-------------------------------------------------------------------------------------------------
bool BuildPath( char *result, int maxlen, const char *path ) {
	strcpy_s( result, maxlen, working_directory );
	strcat_s( result, maxlen, path );
	return true;
}

//-------------------------------------------------------------------------------------------------
FILE *OpenFile( const char *path, const char *mode ) {
	char newpath[MAXPATH];
	BuildPath( newpath, sizeof(newpath), path );
	FILE *f;
	fopen_s( &f, newpath, mode );
	return f;
}

}
