//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

//Authentication Server

#pragma once

#include "system/module.h"

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
		STATE_VERIFYING,
		STATE_DONE,
	};

private:
	State m_state = STATE_LOGIN;

	void Receive( Net::Message &msg ) override;
	void AcceptError( const boost::system::error_code &error ) override;

public:
	AuthStream();

	State GetState() const { return m_state; }
	void SetState( State newstate ) { m_state = newstate; } 
	bool Invalidated() const { return m_state == STATE_DONE; }

};

using AuthStreamPtr = std::shared_ptr<AuthStream>;

//-----------------------------------------------------------------------------
class AuthServer : public System::Module {

	std::unique_ptr<Net::Listener> m_listener;
	
public:
	AuthServer();
	~AuthServer();


protected:
	

private:
	static Net::StreamPtr StreamFactory();

	void OnStart() override;
	void OnShutdown() override;
};

}

