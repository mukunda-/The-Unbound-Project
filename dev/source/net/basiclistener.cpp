//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

#include "stdafx.h"
#include "basiclistener.h"
#include "core.h"

namespace Net {

using boost::asio::ip::tcp;
	 
BasicListener::BasicListener( boost::asio::io_service io_service_, unsigned short port ) : 
					m_acceptor( io_service_, tcp::endpoint( tcp::v4(), port ) ) {
}

BasicListener::BasicListener( unsigned short port ) : 
					m_acceptor( DefaultService()(), tcp::endpoint( tcp::v4(), port ) ) {
}

}
