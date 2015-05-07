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
class Object {

	std::string m_name;
	
public:

	Object( const Stref &name );
	virtual ~Object();

	/** -----------------------------------------------------------------------
	 * Get the name for this object.
	 */
	std::string &GetName() { return m_name; }
	
	/** -----------------------------------------------------------------------
	 * Handle a UI event.
	 */
	void SendEvent( Event::Event &e );

	/** -----------------------------------------------------------------------
	 * Called when an event is triggered.
	 *
	 * @param event The event data.
	 */
	virtual void OnEvent( Event::Event &e );
	
};


}
