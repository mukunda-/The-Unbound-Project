//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//

#pragma once


namespace System { namespace Console {

// execute a system command
// path is relative to game contents folder
void Execute( const char *command_string );

// execute a script file
// path is relative to game contents folder
bool ExecuteScript( const char *file );

// print newline+text to console
// and update terminal
//
void PrintS( const char *text );
void PrintS( const char *format, va_list args );
void Print( const char *format, ... );


// print text tp console without updating terminal
void PrintExS( const char *text );
void PrintExS( const char *format, va_list args );
void PrintEx( const char *format, ... );

}}
