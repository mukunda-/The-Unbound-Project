//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2016, Mukunda Johnson, All rights reserved. =========//

#pragma once

//-----------------------------------------------------------------------------
namespace System {

/** --------------------------------------------------------------------------
 * This is used inside event implementations to define the event info.
 *
 * @param type  The enclosing class name.
 * @param code  Unique code for event.
 * @param name  Name of event
 * @param flags Event flags (EF_* bitmask)
 */
#define SYSTEM_DEFINE_EVENT( code, name, flags ) \
	static const uint32_t CODE  = code;                \
	static const uint32_t FLAGS = flags;               \
	static const Info &INFO() {                        \
		static const Info info( code, name, flags );   \
		return info;                                   \
	}
	
// Event class initializer
#define SYSTEM_EVENT_INIT Event( INFO() )

//-----------------------------------------------------------------------------
struct EventInfo final {
	uint32_t    code;  // unique event code
	std::string name;  // name of event
	uint32_t    flags; // event flags (bitmask) 
	
	EventInfo( uint32_t c, const Stref &n, uint32_t f ) 
		: code(c), name(n), flags(f) {}
};

//-----------------------------------------------------------------------------
class Event {
	
public:
	using Info = EventInfo;
	using Handler = std::function< void( Event &e ) >;
	
	enum {
		// pass to scripting interface, otherwise invisible to user.
		EF_SCRIPTS  = 1
	};

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
	 * Get the event code.
	 */
	uint32_t Code() const { return m_info.code; }

protected:

	/** -----------------------------------------------------------------------
	 * Construct event.
	 * 
	 * @param code The unique code for this event.
	 */
	Event( const Info &info ) : m_info( info ) {}

private:

	const Info &m_info;

};

//-----------------------------------------------------------------------------
}
