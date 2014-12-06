//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

#pragma once

#include "util/feed.h"

namespace Util {
	
	/// -----------------------------------------------------------------------
	/// Format a string.
	///
	/// @param format Template.
	/// @param args   Formatted arguments.
	/// @returns      Formatted string.
	///
	template< typename ... Args >
	std::string Format( const std::string &format, Args ... args ) {
		boost::format formatter(format);
		Util::Feed( formatter, args... );
		return formatter.str();
	}

}
