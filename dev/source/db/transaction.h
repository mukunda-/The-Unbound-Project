//===========================  The Unbound Project  =========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

#pragma once

#include "util/slinkedlist.h"
#include "forwards.h"

namespace DB {

class Transaction {

protected:

	/// -----------------------------------------------------------------------
	/// Perform the actions for this transaction.
	///
	/// @param con SQL Connection to send queries to.
	/// @return true to commit the transaction, false to rollback the
	///         transaction.
	///
	virtual bool Actions( sql::Connection &con ) = 0;

public:
	Transaction()
	{
	}
	 
};

}
