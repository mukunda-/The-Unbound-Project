//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

#pragma once

namespace Net {

/// ---------------------------------------------------------------------------
/// Work is used to keep the network service alive. Before shutdown, the
/// network service will block until all work has completed.
///
class Work {

protected:
	Work();
	virtual ~Work();
};

}