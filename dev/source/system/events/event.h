//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2015, Mukunda Johnson, All rights reserved. =========//

#pragma once

//-----------------------------------------------------------------------------
namespace System {

//-----------------------------------------------------------------------------
struct EventInfo final {
	uint32_t    code;
	std::string name;
	uint32_t    flags;

	EventInfo( uint32_t p_code, const Stref &p_name, uint32_t p_flags ) 
		: code( p_code ), name( p_name ), flags( p_flags )
	{
	}
};

/** ---------------------------------------------------------------------------
 * Macro for defining system events inside their class definitions.
 */
#define SYSTEM_DEFINE_EVENT( code, name, flags ) \
	enum { CODE = code, FLAGS = flags }; \
	const EventInfo &INFO() { \
		static const EventInfo info( code, name, flags ); \
		return info; \
	}
#define SYSTEM_EVENT_INIT Event( CODE )

//-----------------------------------------------------------------------------
class Event {
	 
	const EventInfo &m_info;

	enum {
		EF_SCRIPTS  = 1 // can be seen by user scripts
	};

protected:
	/** -----------------------------------------------------------------------
	 * Construct event.
	 * 
	 * @param code The unique code for this event.
	 */
	Event( const EventInfo &info );

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
			if( T::CODE != m_info.code ) {
				throw std::runtime_error( "Invalid event cast." );
			}
#       endif

		return static_cast<T&>(*this);
	}

	/** -----------------------------------------------------------------------
	 * Get the name of this event.
	 */
	const std::string &Name() const { return m_info.name ); }

	/** -----------------------------------------------------------------------
	 * Get the event code.
	 */
	uint32_t Code() const { return m_info.code; }
};


//-----------------------------------------------------------------------------
}
