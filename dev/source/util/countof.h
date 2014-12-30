//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

#pragma once

template <typename T, size_t S>
int countof( T (&)[S] ) {
	return S;
}
