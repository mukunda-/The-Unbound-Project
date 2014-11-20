//===========================  The Unbound Project  =========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

#pragma once

#include "util/slinkedlist.h"
#include "forwards.h"

namespace DB {
	
class Transaction {
	friend class Manager;
	friend class Connection;

	Connection *parent;

protected:

	enum PostAction {
		NOP,     // do nothing
		COMMIT,  // execute "COMMIT"
		ROLLBACK // execute "ROLLBACK"
	};

	/// -----------------------------------------------------------------------
	/// Perform the actions for this transaction.
	///
	/// @param line SQL Connection to send queries to.
	/// @return command to perform afterwards.
	///
	virtual PostAction Actions( Line &line ) = 0;

	/// -----------------------------------------------------------------------
	/// Called after the transaction is executed.
	///
	/// @param ptr    Pointer that owns "this" (!)
	/// @param failed True if the transaction could not complete.
	///
	virtual void Completed( TransactionPtr ptr, bool failed ) = 0;

public:
	Transaction()
	{
	}
	 
};



}
