//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//

//-------------------------------------------------------------------------------------------------
#pragma once

#include <stdio.h>

//-------------------------------------------------------------------------------------------------
#ifndef IO_FILEBASE_H
#define IO_FILEBASE_H
//-------------------------------------------------------------------------------------------------

namespace IO {

//-------------------------------------------------------------------------------------------------
// get or set the working directory (game folder)
//
// do this at startup!
//
void GetWorkingDirectory( char **path, int maxlen );
void SetWorkingDirectory( const char *path );

//-------------------------------------------------------------------------------------------------
// build a path relative to the working directory
//
// returns true, 100% of the time!
//
bool BuildPath( char *result, int maxlen , const char *path );

//-------------------------------------------------------------------------------------------------
// open a file relative to the program folder
//
FILE *OpenFile( const char *path, const char *mode );

}

//-------------------------------------------------------------------------------------------------
#endif
