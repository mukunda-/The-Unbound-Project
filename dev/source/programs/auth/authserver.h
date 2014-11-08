//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

//Authentication Server

#include "system/program.h"

#include "system/console.h"
#include "net/network_all.h"

namespace User {

class AuthServer : public System::Program {
	 
	class NetEventHandler : public Net::Connection::EventHandler {

		AuthServer &m_parent;
		 
		void AcceptError( Net::Connection &connection, 
						  const boost::system::error_code &error  ) override {
			
			System::Log( "Connection failed to accept." );
			m_parent.AcceptConnection();
		}

		void AcceptedConnection( Net::Connection &connection ) override {

		}

	public:
		
		NetEventHandler( AuthServer &parent ) : m_parent(parent) {}
	};

	NetEventHandler m_event_handler;

	Net::Listener m_listener; 
	
public:
	AuthServer() : m_event_handler(*this), 
				   m_listener( 32791, m_event_handler )
	{

	}

protected:
	void OnStart() override {
		AcceptConnection();
	}

private:
	void AcceptConnection() {
		auto connection = 
			std::shared_ptr<Net::Connection>( 
					new Net::Connection());

	 
		connection->Listen( m_listener );
		System::Log( "Listening." );
	}
};

}

