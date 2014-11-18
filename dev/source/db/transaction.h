//===========================  The Unbound Project  =========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

#pragma once

#include "util/slinkedlist.h"
#include "forwards.h"

namespace DB {

class Transaction : public Util::SLinkedItem<Transaction> {

	Connection &m_connection;

protected:

	/// -----------------------------------------------------------------------
	/// Perform the actions for this transaction.
	///
	/// @return true to commit the transaction, false to rollback the
	///         transaction.
	///
	virtual bool Actions() = 0;

public:
	Transaction( Connection &connection ) : m_connection(connection) 
	{
	}

	/// -----------------------------------------------------------------------
	/// Execute this transaction.
	///
	void Execute();
};

}
