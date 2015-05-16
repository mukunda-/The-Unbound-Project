//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2015, Mukunda Johnson, All rights reserved. =========//

#pragma once

//-----------------------------------------------------------------------------
namespace System {

//-----------------------------------------------------------------------------
class Event {

	const std::string &m_name;
	int                m_id;

protected:
	/** -----------------------------------------------------------------------
	 * Construct event.
	 *
	 * @param name The name (type) of this event.
	 */
	Event( const std::string &name );

public:

	// typically this isn't needed, but just in case?
	//virtual ~Event() {}

	/** -----------------------------------------------------------------------
	 * Get the name of this event.
	 */
	const std::string &Name() const { return m_name; }

	/** -----------------------------------------------------------------------
	 * Get the event ID.
	 */
	int ID() const { return m_id; }
};

//-----------------------------------------------------------------------------
}
