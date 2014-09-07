//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//

#include "stdafx.h"

namespace Util {

int HashString( const char *text, bool case_sensitive ) {
	// todo: improved hashing
	int hash = 0;
	for( int i = 0; text[i]; i++ ) {
		char c = text[i];
		if( !case_sensitive ) {
			c = tolower(c);
		}
		hash += (c+i) << (i & 15);
	}
	return hash;
}

}