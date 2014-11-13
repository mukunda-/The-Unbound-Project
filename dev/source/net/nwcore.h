//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

#pragma once

#include "system/system.h"
#include "stream.h"
#include "types.h"

namespace Net {

	System::Service &DefaultService(); 
	void Run( int threads );
	
	StreamPtr DefaultStreamFactory();

	/// -----------------------------------------------------------------------
	/// Make a remote connection.
	///
	/// This is a blocking function that connects to the specified address.
	///
	/// @param host     Address to connect to
	/// @param service  Service/port number to connect to.
	/// @param factory  Factory to create the stream with.
	/// 
	/// @returns New stream pointer.
	///
	/// @throws boost::system::system_error on failure.
	///
	Stream::ptr Connect( const std::string &host, 
						 const std::string &service,
						 StreamFactory factory = DefaultStreamFactory );
	
	/// -----------------------------------------------------------------------
	/// Make a remote connection asynchronously.
	///
	/// @param host    Remote address.
	/// @param service Service/port number to connect to.
	/// @param handler Event handler to handle completion or error.
	/// @param factory Factory to create the stream with.
	///
	void ConnectAsync( const std::string &host, 
					   const std::string &service, 
					   Asev::Handler &handler,
					   StreamFactory factory = DefaultStreamFactory );


	struct Instance {
	 
		Instance( int threads );
		~Instance();

		System::Service &GetService();

	private:
	
		System::Service m_service;
	};

}
