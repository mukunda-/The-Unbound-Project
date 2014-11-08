//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

#include <stdafx.h>
#include "event.h"

namespace Net {

	namespace Event {
	
		//---------------------------------------------------------------------
		namespace {
			Interface g_null_interface;
		}

		//----------------------------------------------------------------------
		Handler::Handler() {
			m_disabled = false;
		}

		Handler::~Handler() {
			if( !m_disabled ) {
				throw std::runtime_error( "Destructing live event handler." );
			}
		}

		//---------------------------------------------------------------------
		void Handler::Disable() {
			std::lock_guard<std::mutex> lock( m_lock );
			m_disabled = true;
		}

		//---------------------------------------------------------------------
		Lock::Lock( Handler &handler ) :
				m_lock( handler.m_lock ),
				m_interface( handler.m_disabled ? g_null_interface : handler ) {

		}

	}

}