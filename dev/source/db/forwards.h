//===========================  The Unbound Project  =========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

#pragma once

namespace DB {
	struct Endpoint;
	class Connection;
	class Transaction;
	class Line;
	class Manager;

	using TransactionPtr = std::unique_ptr<Transaction>;
	using TransactionHandler = std::function< void( TransactionPtr t ) >;
	using ConnectionPtr = std::unique_ptr<Connection>;
	using LinePtr = std::unique_ptr<Line>;
}
