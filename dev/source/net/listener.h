//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

#pragma once

namespace Net {

class Listener {

	boost::asio::ip::tcp::acceptor m_acceptor;
	
public:
	Listener( boost::asio::io_service io_service_, unsigned short port ); 
	Listener( unsigned short port ); 

	template <typename AcceptHandler> void AsyncAccept( 
				boost::asio::ip::tcp::socket &socket, 
				const AcceptHandler handler ) {

		m_acceptor.async_accept( socket, handler );
	}
};

}
