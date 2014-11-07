//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

//Authentication Server

#include "system/program.h"

#include "system/console.h"

class AuthServer : public System::Program {

	Net::Listener m_listener;
	Net::Connection m_connection;
	
public:
	AuthServer() : m_listener( 32791 ) {

	}

protected:
	void OnStart() override {
		m_connection.Listen( m_listener );

		System::Console::Print("Hi.");
	}

private:

	void AcceptConnection() {
		auto connection = 
			std::shared_ptr<Net::Connection>( 
					new Net::Connection());

		connection->Listen( m_listener );
	}
};
