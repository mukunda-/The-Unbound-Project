//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2015, Mukunda Johnson, All rights reserved. =========//

#pragma once

#include "util/uniquelist.h"
#include "util/stref.h"
#include "event.h"

//-----------------------------------------------------------------------------
namespace Ui {

/** ---------------------------------------------------------------------------
 * The base of all UI entities.
 */
class Object : public Util::UniqueListItem<Object> {

	std::string m_name;
	
public:

	Object( const Stref &name );
	virtual ~Object();

	/** -----------------------------------------------------------------------
	 * Get the name for this object.
	 */
	std::string &GetName() { return m_name; }

	/** -----------------------------------------------------------------------
	 * Called when an event was triggered that this object is registered for.
	 *
	 * @param event The event data.
	 */
	virtual void OnEvent( const Event &e );

	/** -----------------------------------------------------------------------
	 * Register for an event.
	 *
	 * When a matching event fires, OnEvent will be called.
	 *
	 * @param name Name of event.
	 */
	void ListenForEvent( const Stref &name );
};


}
