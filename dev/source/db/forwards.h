//===========================  The Unbound Project  =========================//
//                                                                           //
//========= Copyright � 2014, Mukunda Johnson, All rights reserved. =========//

#pragma once

namespace DB {
	struct Endpoint;
	class Connection;
	class Transaction;
	class Line;

	using ConnectionPtr = std::unique_ptr<Connection>;
}
