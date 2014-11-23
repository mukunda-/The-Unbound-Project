//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//
#pragma once

namespace Util {

	template< class T >
	void Feed( T &output ) { 
	}

	template< class T, typename A, typename ... B >
	void Feed( T &output, A a, B...b ) {
		output % a; 
		Feed( output, b... );
	}

}