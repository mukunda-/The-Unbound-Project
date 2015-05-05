//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2015, Mukunda Johnson, All rights reserved. =========//

#pragma once

/** ---------------------------------------------------------------------------
 * A function to return the number of elements in a static array.
 */
template <typename T, size_t S>
int countof( T (&)[S] ) {
	return S;
}
