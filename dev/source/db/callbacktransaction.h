//===========================  The Unbound Project  =========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

#pragma once

#include "system/callback.h"
#include "db/transaction.h"

namespace DB {

	/// -----------------------------------------------------------------------
	/// Implements Transaction and provides a simple interface to forward
	/// the transaction object to a system callback.
	///
	class CallbackTransaction : public Transaction {
		
	public:
		using Callback = System::Callback<std::shared_ptr<Transaction>, bool>;

		CallbackTransaction( Callback handler ) :
				m_handler( handler ) 
		{
		}

		virtual void Completed( TransactionPtr ptr, bool failed ) {
			m_handler( std::move(ptr), failed );
		}

		template<typename F, typename T>
		static Callback Bind( F func, T thisptr, bool main = true ) 
		{
			return Callback( 
					std::bind( func, thisptr, 
							   std::placeholders::_1, 
							   std::placeholders::_2 
					), main );
		}

	private:
		Callback m_handler;
	};
}
