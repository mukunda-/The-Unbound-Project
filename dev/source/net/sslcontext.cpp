//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2015, Mukunda Johnson, All rights reserved. =========//

#include <stdafx.h>

#include "sslcontext.h"

namespace Net {

SSLContext::SSLContext() : m_context( ssl::context::sslv23 ) {
	
	m_context.set_verify_mode( ssl::verify_peer )
}

}