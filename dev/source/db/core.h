//===========================  The Unbound Project  =========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

#pragma once

namespace DB {

//-----------------------------------------------------------------------------
class Instance {
	std::vector<Connection> m_connections;
public:
	Instance();
};

}