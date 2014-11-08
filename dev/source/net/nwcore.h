//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

#pragma once

#include "system/system.h"
#include "stream.h"

namespace Net {

	System::Service &DefaultService(); 
	void Run( int threads );

	/// -----------------------------------------------------------------------
	/// Make a remote connection.
	///
	/// This is a blocking function that connects to the specified address.
	///
	/// @param host     Address to connect to
	/// @param service  Service/port number to connect to.
	/// 
	/// @returns New stream pointer.
	///
	/// @throws boost::system::system_error on failure.
	///
	Stream::ptr Connect( const std::string &host, 
						 const std::string &service );
	
	/// -----------------------------------------------------------------------
	/// Make a remote connection asynchronously.
	///
	/// @param host    Remote address.
	/// @param service Service/port number to connect to.
	/// @param handler Event handler to handle completion or error.
	///
	void ConnectAsync( const std::string &host, 
					   const std::string &service, 
					   Event::Handler &handler );

	struct Instance {
	 
		Instance( int threads );
		~Instance();

		System::Service &GetService();

	private:
	
		System::Service m_service;
	};

}
