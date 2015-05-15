//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2015, Mukunda Johnson, All rights reserved. =========//

#pragma once

//-----------------------------------------------------------------------------
namespace System {

//-----------------------------------------------------------------------------
class Event {

	int m_type;

protected:
	/** -----------------------------------------------------------------------
	 * Construct event.
	 *
	 * @param type The type id returned from the event registration.
	 */
	Event( int type ) : m_type( type ) {}

public:

	// typically this isn't needed, but just in case.
	virtual ~Event() {}

	/** -----------------------------------------------------------------------
	 * Get the name of this event.
	 */
	const std::string &Name();

};

//-----------------------------------------------------------------------------
}
