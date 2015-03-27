//===========================  The Unbound Project  =========================//
//                                                                           //
//========= Copyright © 2015, Mukunda Johnson, All rights reserved. =========//

#pragma once

namespace DB {
	 
/// ---------------------------------------------------------------------------
/// Work is used to keep track if the database service is in use.
///
class Work {
	
protected:
	Work();
	virtual ~Work();
};

}
