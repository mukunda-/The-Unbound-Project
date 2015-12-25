//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2015, Mukunda Johnson, All rights reserved. =========//

#include "stdafx.h"
#include "stream.h"

namespace Net {

	//-------------------------------------------------------------------------
	Stream::SendLock::SendLock( Net::Stream &parent, 
								boost::asio::streambuf &buffer ) : 
			m_parent(parent), 
			m_buffer(buffer),
			m_locked(true) {
			
	}
	
	//-------------------------------------------------------------------------
	Stream::SendLock::SendLock( SendLock &&lock ) :
			m_locked(lock.m_locked), m_parent(lock.m_parent),
			m_buffer(lock.m_buffer) {

		lock.m_locked = false;
	}

	//-------------------------------------------------------------------------
	Stream::SendLock::~SendLock() {
		if( m_locked ) {
			m_parent.ReleaseSendBuffer( true );
		}
	}
}