//===========================  The Unbound Project  =========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

#pragma once

#include "util/feed.h"

namespace Console {

template <typename ...T>
void Print( const std::string &format, T...args ) {
	boost::format formatter( format );
	Util::Feed( formatter, args... );

}

void Print( const std::string &text );

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

}
