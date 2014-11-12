//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

//Authentication Server

#include "system/program.h"

#include "system/console.h"
#include "net/stream.h"
#include "net/listener.h"
#include "net/events.h"

namespace User {

class AuthStream : public Net::Stream {

	enum {
		STATE_LOGIN,
		STATE_DONE,
	};

	int m_state;

public:
	AuthStream();
};

class AuthServer : public System::Program {
	 
	class NetEventHandler : public Net::Events::Stream::Handler {

		AuthServer &m_parent;
		 
		void AcceptError( Net::Stream::ptr &stream, 
						  const boost::system::error_code &error  ) override;
		void Accepted( Net::Stream::ptr &stream ) override;

	public:
		
		NetEventHandler( AuthServer &parent );
	};

	NetEventHandler m_event_handler;
	Net::Listener m_listener; 
	
public:
	AuthServer();
	~AuthServer();

protected:
	void OnStart() override;

private:
	static Net::StreamPtr StreamFactory();
};

}

