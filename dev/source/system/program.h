//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

#pragma once

namespace System {

class Program {

public:
	virtual void OnStart() {};
	virtual void OnStop() {};
	virtual ~Program() {}
};

}