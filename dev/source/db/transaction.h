//===========================  The Unbound Project  =========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

#pragma once
 
#include "work.h"
#include "forwards.h"

namespace DB {
	
/// ---------------------------------------------------------------------------
/// The transaction class provides an interface to execute a transaction
/// in the database. A transaction contains one or more queries that 
/// don't rely on previous or future operations.
///
/// The manager will try to execute the transaction and retry if there is
/// a recoverable failure until it succeeds or becomes impossible.
///
class Transaction : public Work {
	friend class Manager;
	friend class Connection;

	Connection *m_parent = nullptr;
	int m_mysql_error = 0;

protected:

	enum PostAction {
		NOP,     // do nothing
		COMMIT,  // execute "COMMIT"
		ROLLBACK, // execute "ROLLBACK"
		FAIL     // cancel the transaction
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

	/// -----------------------------------------------------------------------
	/// Return type information about this transaction. Not used by the
	/// system.
	///
	virtual int Type() { return 0; }
	
	/// -----------------------------------------------------------------------
	/// Return desired SQL autocommit option.
	///
	virtual bool AutoCommit() { return true; }

public:
	Transaction() {}
	virtual ~Transaction() {}

	/// -----------------------------------------------------------------------
	/// On failure, get the mysql error code.
	///
	int GetMySQLError() { return m_mysql_error; }
};

}
