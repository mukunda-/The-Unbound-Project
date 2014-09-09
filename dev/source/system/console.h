//===========================  The Unbound Project  =========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

#pragma once


namespace System { namespace Console {

/// ---------------------------------------------------------------------------
/// Execute a command.
///
void Execute( const char *command_string );

/// ---------------------------------------------------------------------------
/// Execute a script file
///
/// \param file Path to script file, relative to game contents folder.
///
bool ExecuteScript( const char *file );

/// ---------------------------------------------------------------------------
/// Print newline+text to the console
///
void PrintS( const char *text );
void PrintS( const char *format, va_list args );
void Print( const char *format, ... );

/// ---------------------------------------------------------------------------
/// Print text to the console without updating the terminal.
///
/// i.e. doesn't actually print anything until an update.
///
void PrintExS( const char *text );
void PrintExS( const char *format, va_list args );
void PrintEx( const char *format, ... );

}}
