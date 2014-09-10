//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//

#include "stdafx.h" 


#include "system/commands.h"
#include "system/variables.h"
#include "util/stringles.h"
#include "util/codetimer.h"
#include "util/fopen2.h"

#if defined SERVER
#include "system/server/serverconsole.h" // todo?
#endif


//-------------------------------------------------------------------------------------------------
namespace System { namespace Console {

//-------------------------------------------------------------------------------------------------
void Execute( const char *command_string ) {

	// copy command
	char command[1024];
	Util::CopyString( command, command_string );
	Util::TrimString(command);
	{
		// strip comment
		char *comment = strstr( command, "//" );
		if( comment ) comment[0] = 0;
	}

	char name[64];
	const char *next = Util::BreakString( command_string, name );
	if( name[0] == 0 ) {
		System::Console::Print( "" );
		return;
	}

	System::Console::Print( "\n>>> %s", command_string );
	
	if( TryExecuteCommand( command_string ) ) {
		return;
	}
	
	
	System::Variable *var = System::Variable::Find( name );
	if( !var ) {

		System::Console::Print( "Unknown command: \"%s\"", name );
		return;
	} 

	char value[512];
	Util::CopyString( value, next );
	Util::TrimString( value );
	Util::StripQuotes( value );
	
	if( Util::StrEmpty( value ) ) {
		var->PrintInfo();
	} else {
		var->SetString( value, false );
		//System::Console::Print( "cvar \"%s\" set to \"%s\".", var->Name().c_str(), value );
	}
}

//-------------------------------------------------------------------------------------------------
bool ExecuteScript( const char *file ) {
	FILE *f = fopen2( file, "r" );
	if( !f ) {
		System::Console::Print( "Script not found: \"%s\"", file );
		return false;
	}

	char line[1024];
	System::Console::Print( "Executing script: \"%s\"", file );

	Util::CodeTimer timer;

	while( !feof(f) ) {
		fgets( line, sizeof line, f );
		Execute( line );
	}

	System::Console::Print( "Finished executing script: \"%s\", time=%s", file, 
		Util::RoundDecimal( timer.Duration(),2 ).c_str() );

	return true;
}

//-------------------------------------------------------------------------------------------------
void PrintS( const char *text ) {
#if defined SERVER
	System::ServerConsole::PrintToWindow( text, true );
#else 
	// TODO
#endif
}

//-------------------------------------------------------------------------------------------------
void Print( const char *format, ... ) {
	va_list argptr;
	va_start( argptr, format );
#if defined SERVER
	System::ServerConsole::PrintToWindow( format, true, argptr );
#else

#endif

	va_end( argptr );
}

//-------------------------------------------------------------------------------------------------
void PrintS( const char *format, va_list args ) {
#if defined SERVER
	System::ServerConsole::PrintToWindow( format, true, args );
#else

#endif
}


//-------------------------------------------------------------------------------------------------
void PrintExS( const char *text ) {
#if defined SERVER
	System::ServerConsole::PrintToWindow( text, false );
#else

#endif
}

//-------------------------------------------------------------------------------------------------
void PrintEx( const char *format, ... ) {
	va_list argptr;
	va_start( argptr, format );
#if defined SERVER
	System::ServerConsole::PrintToWindow( format, false, argptr );
#else

#endif
	va_end( argptr );
}


//-------------------------------------------------------------------------------------------------
}}
