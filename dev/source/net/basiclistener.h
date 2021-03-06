//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright � 2015, Mukunda Johnson, All rights reserved. =========//

#pragma once

#include "work.h"

namespace Net {

/** ---------------------------------------------------------------------------
 * A wrapper for an ASIO tcp acceptor.
 */
class BasicListener : public Work {

	boost::asio::ip::tcp::acceptor m_acceptor;
	
public:
	BasicListener( boost::asio::io_service io_service_, unsigned short port ); 
	BasicListener( unsigned short port ); 
	
	template <typename AcceptHandler> void AsyncAccept( 
				boost::asio::ip::tcp::socket &socket, 
				const AcceptHandler handler ) {

		m_acceptor.async_accept( socket, handler );
	}

	virtual ~BasicListener() {}
};

}
