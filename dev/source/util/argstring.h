//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//

#pragma once

#include "util/stringles.h"

//-----------------------------------------------------------------------------
namespace Util {

//-----------------------------------------------------------------------------
struct ArgString {
	
	std::vector<std::string> args;

	ArgString( const std::string &input ) {
		const char *source = input.c_str();
		char dest[256];
		for(;;) {
			source = Util::BreakString( source, dest );
			if( dest[0] == 0 ) break;
			args.push_back( dest );	
		}
	}
};

}