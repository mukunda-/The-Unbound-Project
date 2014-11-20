//===========================  The Unbound Project  =========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

#pragma once

#include "system/callback.h"
#include "db/transaction.h"

namespace DB {

	template<typename ... Args>
	class CallbackTransaction : public Transaction {
		
	public:
		using Callback = System::Callback<Args...>;

		CallbackTransaction( System::Callback<Args...> handler ) :
				m_handler( handler ) {
			
		}
		 
	private:
		Callback m_handler;
	};
}