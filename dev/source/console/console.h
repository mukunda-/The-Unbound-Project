//===========================  The Unbound Project  =========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

#pragma once

#include "util/format.h"
#include "util/stringref.h"

namespace Console {

/// ---------------------------------------------------------------------------
/// Printing implementation.
///
class PrintHandler {
	
public:

	/// -----------------------------------------------------------------------
	/// Implementation: Print some text.
	///
	/// @param text  Text to print
	/// @param error True if this is an error message.
	///
	virtual void Print( const char *text, bool error );

	using ptr = std::shared_ptr<PrintHandler>;
};

/// ---------------------------------------------------------------------------
/// Print text to the console
///
/// @param format Text with formatting symbols.
/// @param args   Formatted arguments.
///
template< typename ... T >
void Print( const Util::StringRef &format, T...args ) {
	Print( Util::Format( format, args... ));
}

template< typename ... T >
void PrintErr( const Util::StringRef &format, T...args ) {
	PrintErr( Util::Format( format, args... ));
}

template< typename ... T >
void PrintEx( const Util::StringRef &format, T...args ) {
	PrintEx( Util::Format( format, args... ));
}

template< typename ... T >
void PrintErrEx( const Util::StringRef &format, T...args ) {
	PrintEx( Util::Format( format, args... ));
}

/// ---------------------------------------------------------------------------
/// Print unformatted text.
///
void Print( const Util::StringRef &text );
void PrintErr( const Util::StringRef &text );

/// ---------------------------------------------------------------------------
/// Print unformatted text without adding a newline.
///
void PrintEx( const Util::StringRef &text );
void PrintErrEx( const Util::StringRef &text );

/// ---------------------------------------------------------------------------
/// Change the handler for printing stuff.
///
void SetPrintHandler( PrintHandler::ptr handler );

/// ---------------------------------------------------------------------------
/// Main controller
///
class Instance {
private:
	PrintHandler::ptr m_printhandler;
	std::mutex m_print_mutex;

public:
	Instance();
	~Instance();
	
	void Print( const char *text, bool newline, bool error );
	void SetPrintHandler( PrintHandler::ptr handler );
};

}
