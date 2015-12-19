//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2015, Mukunda Johnson, All rights reserved. =========//

#pragma once

#include "system/system.h"
#include "system/module.h"
#include "stream.h"
#include "types.h"

namespace Net {

	System::Service &DefaultService(); 
	void Run( int threads ); 

	/** -----------------------------------------------------------------------
	 * Make a remote connection.
	 *
	 * This is a blocking function that connects to the specified address.
	 *
	 *  @param host     Address to connect to
	 *  @param service  Service/port number to connect to.
	 *  @param factory  Factory to create the stream with.
	 *  
	 *  @returns New stream pointer.
	 * 
	 *  @throws boost::system::system_error on failure.
	 */ 
	Stream::ptr Connect( const std::string &host, 
						 const std::string &service,
						 StreamFactory factory );
	
	/** -----------------------------------------------------------------------
	 *  Make a remote connection asynchronously.
	 * 
	 *  @param host    Remote address.
	 *  @param service Service/port number to connect to. 
	 *  @param factory Factory to create the stream with.
	 */ 
	void ConnectAsync( const std::string &host, 
					   const std::string &service,  
					   StreamFactory factory );

	//-------------------------------------------------------------------------
	struct Instance : public System::Module {
	 
		Instance();
		~Instance();

		System::Service &GetService();

	private:
		friend class Stream;
		friend class Work;
	
		static void LockingFunction( int mode, int n, const char *file, 
									 int line );

		std::mutex m_lock;

		// locks for CRYPTO locking callback
		std::unique_ptr<std::mutex[]> m_crypto_locks;
		

		boost::asio::strand m_ssl_strand;
		
		boost::asio::strand &GetSSLStrand() { return m_ssl_strand; }
	};

}
