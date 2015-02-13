//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2015, Mukunda Johnson, All rights reserved. =========//

#include <stdafx.h>

#include "sslcontext.h"

using namespace std;

//-----------------------------------------------------------------------------
namespace Net {

//-----------------------------------------------------------------------------
SSLContext::SSLContext() : m_context( boost::asio::ssl::context::sslv23 ) {
	
}

//-----------------------------------------------------------------------------
void SSLContext::SetupServer( const Util::StringRef &cert_file, 
							  const Util::StringRef &pem_file ) {

	m_context.set_options( boost::asio::ssl::context::default_workarounds |
							boost::asio::ssl::context::no_sslv2 |
							boost::asio::ssl::context::single_dh_use );

	m_context.use_certificate_chain_file( cert_file );
	m_context.use_private_key_file( pem_file, boost::asio::ssl::context::pem );

	// what is this for?
	m_context.use_tmp_dh_file( "dh512.pem" );
	 
}

//-----------------------------------------------------------------------------
void SSLContext::SetupClient( const Util::StringRef &pem_file ) {
	m_context.load_verify_file( pem_file );	
	 
}



}