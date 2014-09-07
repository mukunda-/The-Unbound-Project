//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//

#include "stdafx.h"
#include "network/listener.h"
#include "network/nwcore.h"

using boost::asio::ip::tcp;

namespace Network {



Listener::Listener( boost::asio::io_service io_service_, unsigned short port ) : 
					m_acceptor( io_service_, tcp::endpoint( tcp::v4(), port ) ) {
	
}

Listener::Listener( unsigned short port ) : 
					m_acceptor( DefaultService()(), tcp::endpoint( tcp::v4(), port ) ) {
	
}

}
