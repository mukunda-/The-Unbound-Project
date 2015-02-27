//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

#include <stdafx.h>
#include "authserver.h"
#include "protocol.h"
#include "proto/auth/login.pb.h"

#include "db/core.h"
#include "db/transaction.h"
#include "db/statement.h"
#include "hash/md5.h"

//-----------------------------------------------------------------------------
namespace User {

AuthServer *g_instance;
	
/*
	Login process:

	Client sends username and a hashed password (weak/sha-256)

	Only the client knows their actual password, the hash of it is treated
	as their password on the server side

	The server looks up the clients credentials and responds with an
	error if they dont exist
	  
	The server verifies the password received with the hashed
	password in the database. (strong/bcrypt)
	  
	The server responds with an error if the credentials are invalid, or
	generates an authentication token and sends it to the client.

*/

namespace {

	/** -----------------------------------------------------------------------
		* Hash a username for indexing into the Account database table.
		*
		* @param input Username to hash
		* @returns 32-bit unsigned hash
		*/
	uint32_t HashUsername( const Util::StringRef &input ) {
		// hash is 16 chars of md5

		return std::stoul( Hash::md5(input).substr(0,8), nullptr, 16 );
	}
}

//-----------------------------------------------------------------------------
class LoginQuery : public DB::Transaction {

public:
	/** -----------------------------------------------------------------------
	 * Create a login transaction.
	 *
	 * @param stream Stream that is making the request. This stream will
	 *               be sent a response.
	 * @param username Username given by client.
	 * @param password Password given by client.
	 */
	LoginQuery( std::shared_ptr<AuthStream> stream, 
		        const Util::StringRef &username, 
				const Util::StringRef &password ) :

		m_username( username ), 
		m_password( password ), 
		m_stream( stream ) 
	{

	}

private:
	//-------------------------------------------------------------------------
	PostAction Actions( DB::Line &line ) override {

		int userhash = HashUsername( m_username );

		auto statement = line.CreateStatement();
		auto result = statement->ExecuteQuery( 
			"SELECT accountid, password FROM Accounts "
			"WHERE userhash=%d AND username=%s", 
			userhash, m_username );

		if( !result->next() ) {
			// unknown user!
			return NOP;
		}

		std::string password = result->getString( 2 );


		//bcrypt_check( password, fewaoweouseri

	} 

	//-------------------------------------------------------------------------
	void Completed( DB::TransactionPtr ptr, bool failed ) override {
		// todo

	}

	std::shared_ptr<AuthStream> m_stream;
	std::string m_username;
	std::string m_password;
};

//-----------------------------------------------------------------------------
struct AuthMessage {
		   
	static uint16_t ID( Net::LidStream::Message &msg ) {
		return msg.Header() & 0x3FFF;
	}
	static uint16_t Extra( Net::LidStream::Message &msg ) {
		return msg.Header() >> 14;
	}
};

//-----------------------------------------------------------------------------
AuthStream::AuthStream() {
	m_state = STATE_LOGIN;
}
	 
	
//-----------------------------------------------------------------------------
void AuthStream::AcceptError( const boost::system::error_code &error ) {
			
	System::Log( "A connection failed to accept. %d: %s", 
					error.value(), error.message() ); 
}
	  
//-----------------------------------------------------------------------------
void AuthStream::Receive( Net::Message &netmsg ) {
		 
	if( m_state == STATE_LOGIN ) {

		auto &msg = netmsg.Cast<Net::LidStream::Message>();
			
		if( AuthMessage::ID(msg) == Net::Proto::ID::Auth::LOGIN ) {

			m_state = STATE_VERIFYING;

			// user wants to log in.
			Net::Proto::Auth::Login buffer;
			msg.Parse( buffer );
			Console::Print( "Login: %s : %s", buffer.username(), buffer.password() );

			// todo, validate input

			DB::TransactionPtr transaction( 
				new LoginQuery( 
					std::static_pointer_cast<AuthStream>(shared_from_this()), 
					buffer.username(), 
					buffer.password() 
				));

		} else {
			// bad client.
			m_state = STATE_DONE;
			Close();
		}
	}
}

//-----------------------------------------------------------------------------
AuthServer::AuthServer() {

	g_instance = this;
	Console::Print( "Listening." );
}

//-----------------------------------------------------------------------------
AuthServer::~AuthServer() {
	g_instance = nullptr;
}

//-----------------------------------------------------------------------------
void AuthServer::OnStart() {
	m_listener.reset( new Net::Listener( 32791, StreamFactory ));
}

//-----------------------------------------------------------------------------
void AuthServer::OnStop() {
	m_listener = nullptr;
}

//-----------------------------------------------------------------------------
Net::Stream::ptr AuthServer::StreamFactory() {
	return std::make_shared<AuthStream>();
}

//-----------------------------------------------------------------------------
}
