//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//

#include "stdafx.h" 


#include "system/commands.h"
#include "system/variables.h"
#include "util/stringles.h"
#include "util/codetimer.h"
#include "util/fopen2.h"
 
//-------------------------------------------------------------------------------------------------
namespace System { namespace Console {


//-------------------------------------------------------------------------------------------------
bool ExecuteScript( const char *file ) {
	FILE *f = fopen2( file, "r" );
	if( !f ) {
		::Console::Print( "Script not found: \"%s\"", file );
		return false;
	}

	char line[1024];
	::Console::Print( "Executing script: \"%s\"", file );

	Util::CodeTimer timer;

	while( !feof(f) ) {
		fgets( line, sizeof line, f );
		Execute( line );
	}

	::Console::Print( "Finished executing script: \"%s\", time=%s", file, 
		Util::RoundDecimal( timer.Duration(),2 ).c_str() );

	return true;
}

/*
//-------------------------------------------------------------------------------------------------
void PrintS( const char *text ) {
#if defined UB_SERVER
	System::ServerConsole::PrintToWindow( text, true );
#else 
	// TODO
#endif
}

//-------------------------------------------------------------------------------------------------
void Print( const char *format, ... ) {
	va_list argptr;
	va_start( argptr, format );
#if defined UB_SERVER
	System::ServerConsole::PrintToWindow( format, true, argptr );
#else

#endif

	va_end( argptr );
}

//-------------------------------------------------------------------------------------------------
void PrintS( const char *format, va_list args ) {
#if defined UB_SERVER
	System::ServerConsole::PrintToWindow( format, true, args );
#else

#endif
}


//-------------------------------------------------------------------------------------------------
void PrintExS( const char *text ) {
#if defined UB_SERVER
	System::ServerConsole::PrintToWindow( text, false );
#else

#endif
}

//-------------------------------------------------------------------------------------------------
void PrintEx( const char *format, ... ) {
	va_list argptr;
	va_start( argptr, format );
#if defined UB_SERVER
	System::ServerConsole::PrintToWindow( format, false, argptr );
#else

#endif
	va_end( argptr );
}

*/

//-------------------------------------------------------------------------------------------------
}}
