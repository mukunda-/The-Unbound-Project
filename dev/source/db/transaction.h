//===========================  The Unbound Project  =========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

#pragma once

namespace DB {

class Transaction {

protected:
	/// -----------------------------------------------------------------------
	/// (Implementation) Perform the actions for this transaction.
	///
	/// @return true to commit the transaction, false to rollback the
	///         transaction.
	virtual bool Actions() = 0;

public:

	void Execute();
};

}