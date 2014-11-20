//===========================  The Unbound Project  =========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

#pragma once

#include "system.h"

namespace System {
	
	/// -----------------------------------------------------------------------
	/// Callback object to post a task on the system service.
	///
	template<typename... Args>
	class Callback {
		
		bool m_main_strand;
		std::function<void(Args...)> m_handler;

	public:

		/// -------------------------------------------------------------------
		/// Create a callback.
		///
		/// @param handler Handler to execute. The task function.
		/// @param main    Run this task on the main/system thread.
		///
		Callback( std::function<void(Args...)> handler, bool main = true ) :
				m_handler( std::move(handler) ),
				m_main_strand( main ) {
		} 
		
		template<typename T>
		Callback( T handler ) : 
				Callback( handler, true )
		{
		}
		 
		/// -------------------------------------------------------------------
		/// Execute this callback.
		///
		/// @param args Argument list.
		///
		void operator()( Args... args ) {
			Post( std::bind( m_handler, args... ), m_main_strand, 0 );
		}

		static Callback<Args...> Bind() {
			return Callback<Args...>
		}

	};
}
