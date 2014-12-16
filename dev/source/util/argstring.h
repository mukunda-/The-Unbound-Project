//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

#pragma once

#include "util/stringles.h"
#include "util/stringref.h"

//-----------------------------------------------------------------------------
namespace Util {

/// ---------------------------------------------------------------------------
/// A class to parse a command string.
///
class ArgString {
	std::vector<std::string> args;

public:

	/// -----------------------------------------------------------------------
	/// Parse an argument string.
	///
	ArgString( const Util::StringRef input ) {
		const char *source = *input;
		char dest[256];
		for(;;) {
			source = Util::BreakString( source, dest );
			if( dest[0] == 0 ) break;
			args.push_back( dest );	
		}
	}
	
	/// -----------------------------------------------------------------------
	/// @returns argument list.
	///
	const std::vector<std::string> &Args() const {
		return args;
	} 
	
	/// -----------------------------------------------------------------------
	/// @returns argument list.
	///
	std::vector<std::string> &Args() {
		return args;
	} 
	
	/// -----------------------------------------------------------------------
	/// @returns number of arguments parsed.
	///
	int Count() const {
		return (int)args.size();
	}

	/// -----------------------------------------------------------------------
	/// Read an argument
	///
	/// @param index Index of argument.
	/// @returns argument, or "" if the index is out of bounds.
	///
	const std::string &operator[]( int index ) const {
		if( index < 0 || index >= Count() ) {
			static const std::string empty_arg;
			return empty_arg;
		}
		return args[index];
	}
};

}
