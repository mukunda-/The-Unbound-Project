//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2015, Mukunda Johnson, All rights reserved. =========//

#pragma once

/** ---------------------------------------------------------------------------
 * Macro for defining system events inside their class definitions.
 */
#define SYSTEM_DEFINE_EVENT( code, name ) \
	enum { CODE = code }; \
	const std::string &NAME() { \
		static const std::string event_name = name; \
		return event_name; \
	}
#define SYSTEM_EVENT_INIT Event( NAME(), CODE )

//-----------------------------------------------------------------------------
namespace System {

//-----------------------------------------------------------------------------
class Event {
	 
	const uint32_t m_code;

protected:
	/** -----------------------------------------------------------------------
	 * Construct event.
	 * 
	 * @param code The unique code for this event.
	 */
	Event( uint32_t code );

public:

	// typically this isn't needed, but just in case?
	//virtual ~Event() {}
	// nah, event structs should be POD, created and deleted on the stack only

	/** -----------------------------------------------------------------------
	 * Upcast a base event to it's derived type.
	 *
	 * This checks the event code for invalid casts in debug mode.
	 */
	template< typename T > T &Cast() {

#       ifdef _DEBUG
			if( T::CODE != m_code ) {
				throw std::runtime_error( "Invalid event cast." );
			}
#       endif

		return static_cast<T&>(*this);
	}

	/** -----------------------------------------------------------------------
	 * Get the name of this event.
	 */
	const std::string &Name() const;

	/** -----------------------------------------------------------------------
	 * Get the event code.
	 */
	uint32_t Code() const { return m_code; }
};

//-----------------------------------------------------------------------------
}
