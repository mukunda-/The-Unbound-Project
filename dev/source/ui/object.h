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

	// name of the object, used to quickly find it if needed
	// later.
	std::string m_name;

	uint8_t m_workless = 0;
	
	//-------------------------------------------------------------------------
public:
	
	Object( const Stref &name );
	virtual ~Object();

	/** -----------------------------------------------------------------------
	 * Get the name for this object.
	 */
	const std::string &GetName() const { return m_name; }
	
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
