//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2015, Mukunda Johnson, All rights reserved. =========//

#pragma once

#include "util/uniquelist.h"

//-----------------------------------------------------------------------------
namespace Ui {

/** ---------------------------------------------------------------------------
 * The base of all UI entities.
 */
class Object : public Util::UniqueListItem<Object> {
	
public:
	void OnEvent( const Event &event );
};


}
