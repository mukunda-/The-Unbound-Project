//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2015, Mukunda Johnson, All rights reserved. =========//

#pragma once

namespace System {

/** ---------------------------------------------------------------------------
 * A Pulse is a source that posts a task on a timer.
 */
class Pulse {
 
public:

	/** -----------------------------------------------------------------------
	 * Create a pulse.
	 *
	 * @param frequency Frequency at which the event should be generated.
	 */
	Pulse( float frequency );
	virtual ~Pulse();
	
};

}