//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2015, Mukunda Johnson, All rights reserved. =========//

#pragma once

//-----------------------------------------------------------------------------
namespace System {

//-----------------------------------------------------------------------------
struct EventInfo final {
	uint32_t    code;  // unique event code
	std::string name;  // name of event
	uint32_t    flags; // event flags (bitmask) 
	
	EventInfo( uint32_t c, const Stref &n, uint32_t f ) 
		: code(c), name(n), flags(f)
	{ 
	}
};

// ---------------------------------------------------------------------------
// Macros for defining event info.
/** --------------------------------------------------------------------------
 * This is placed inside the class definition.
 *
 * @param code  Unique code for event.
 * @param name  Name of event
 * @param flags Event flags (EF_* bitmask)
 */
#define SYSTEM_DEFINE_EVENT( code, name, flags )    \
	static const uint32_t CODE  = code;             \
	static const uint32_t FLAGS = flags;            \
	static EventInfo &INFO() {                      \
		static EventInfo info( code, name, flags ); \
		return info;                                \
	}
	
// Event class initializer
#define SYSTEM_EVENT_INIT Event( INFO() )

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
	
	using Handler = std::function< void( Event &e ) >;

	// typically this isn't needed, but just in case?
	//virtual ~Event() {}
	// nah, event structs should be POD, created and deleted on the stack only

	/** -----------------------------------------------------------------------
	 * Upcast a base event to it's derived type.
	 *
	 * This checks for invalid casts in debug mode.
	 */
	template< typename T > T &Cast() {

#       ifdef _DEBUG
			if( T::INFO().name != m_info.name ) {
				throw std::runtime_error( "Invalid event cast." );
			}
#       endif

		return static_cast<T&>(*this);
	}

	/** -----------------------------------------------------------------------
	 * Get the name of this event.
	 */
	const std::string &Name() const { return m_info.name; }

	/** -----------------------------------------------------------------------
	 * Get the event ID.
	 */
	//uint32_t ID() const { return m_info.id; }

	/** -----------------------------------------------------------------------
	 * Get the event code.
	 */
	uint32_t Code() const { return m_info.code; }
};

//-----------------------------------------------------------------------------
}
