//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

//Authentication Server

#pragma once

#include "system/program.h"

#include "system/console.h"
#include "net/core.h"
#include "net/lidstream.h"
#include "net/listener.h"
#include "net/events.h"

namespace User {

//-----------------------------------------------------------------------------
class AuthStream : public Net::LidStream {

public:
	enum State{
		STATE_LOGIN,
		STATE_DONE,
	};

private:
	State m_state = STATE_LOGIN;

public:
	AuthStream();

	State GetState() const { return m_state; }
	void SetState( State newstate ) { m_state = newstate; } 
	bool Invalidated() const { return m_state == STATE_DONE; }
};

using AuthStreamPtr = std::shared_ptr<AuthStream>;

class AuthServer : public System::Program {
	 
	class NetEventHandler : public Net::Events::Stream::Handler {

		AuthServer &m_parent;
		 
		void AcceptError( Net::Stream::ptr &stream, 
						  const boost::system::error_code &error  ) override;
		void Accepted( Net::Stream::ptr &stream ) override;
		void Receive( Net::Stream::ptr &stream, Net::Message &msg ) override;

	public:
		
		NetEventHandler( AuthServer &parent );
	};

	Net::Instance i_network;

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

