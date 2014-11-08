//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

#pragma once

namespace Net {
	 
class EventLatch : public std::lock_guard<std::mutex> {

public:
	EventLatch( EventHandler &handler );

};

}
