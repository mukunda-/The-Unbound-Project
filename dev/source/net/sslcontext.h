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

	/// -----------------------------------------------------------------------
	/// Create an SSL context which is initialized using SetupServer or
	/// SetupClient.
	///
	SSLContext();
	
	/// -----------------------------------------------------------------------
	/// Set up a server context.
	///
	/// After calling this, the context may be used to secure sockets that
	/// will accept connections.
	///
	/// @param cert_file PEM file containing the server's certificate.
	/// @param key_file  PEM file containing the server's private key.
	///
	void SetupServer( const Util::StringRef &cert_file, 
					  const Util::StringRef &key_file );
	
	/// -----------------------------------------------------------------------
	/// Set up a client context.
	///
	/// After calling this, the context may be used to secure sockets
	/// that will make outgoing connections.
	///
	/// @param cert_file PEM file containing the certificate to validate the
	///                  server's certificate.
	///
	void SetupClient( const Util::StringRef &cert_file );

	/// -----------------------------------------------------------------------
	/// @returns the underlying ASIO ssl context.
	///
	boost::asio::ssl::context& operator()() {
		return m_context;
	}

private:

};

}
