//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

// Network Connection Class

#pragma once

#include "listener.h"
#include "packetfifo.h"
#include "nwcore.h"
#include "util/minmax.h"

namespace Net {
  
/// ---------------------------------------------------------------------------
/// A Connection manages a TCP socket with boost::asio.
///
class Connection {  

public:
	 
private:
	 
	// the stream stays alive if this class is destroyed to 
	// cleanly close the connection and finish sending data.
	std::shared_ptr<Stream> m_stream;

	// where this connection is coming from/going to
	std::string m_hostname;
	
	EventHandler *m_event_handler; 
	
	//int FireEvent( EventType type, void *data=0 );
	
	void *m_userdata; 

	//-------------------------------------------------------------------------
protected:

public:  
	//-------------------------------------------------------------------------
	Connection(); 
	~Connection();
	 
	/// -----------------------------------------------------------------------
	/// Make a remote connection.
	///
	/// This is a blocking function that connects to the specified address.
	///
	/// @param host Address to connect to
	/// @param service Service/port number to connect to.
	/// @throws boost::system::system_error on failure.
	///
	void Connect( const std::string &host, const std::string &service );

	/// -----------------------------------------------------------------------
	/// Make a remote connection asynchronously.
	///
	/// This starts a connection task that completes in the background.
	/// 
	/// Fires a connection error event on failure.
	///  
	void ConnectAsync( const std::string &host, const std::string &service );
	
	// To close a connection, destruct the Connection object.
	//void Close();
	
};


}
