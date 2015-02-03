//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2015, Mukunda Johnson, All rights reserved. =========//

#pragma once

namespace Net {


class SSLContext {

public:
	boost::asio::ssl::context &m_context;

	SSLContext();
private:
	
};

}
