//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2015, Mukunda Johnson, All rights reserved. =========//

#pragma once

#include "util/stringref.h"

//-----------------------------------------------------------------------------
namespace Net {

//-----------------------------------------------------------------------------
class SSLContext {

public:
	boost::asio::ssl::context m_context;

	SSLContext();
	
	void SetupServer( const Util::StringRef &pem );
	void SetupClient( const Util::StringRef &pem );

	boost::asio::ssl::context& operator()() {
		return m_context;
	}

private:
	
};

}
