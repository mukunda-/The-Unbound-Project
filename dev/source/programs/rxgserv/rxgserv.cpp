
#include "stdafx.h"
#include "rxgserv.h"
#include "system/console.h"
#include "system/commands.h"
#include "util/stringles.h"

//-----------------------------------------------------------------------------
namespace User { namespace RXGServ {

RXGServ *g_rxgserv;


//-----------------------------------------------------------------------------
RXGServ::RXGServ() :
		m_listener( LISTEN_PORT, StreamFactory, &m_netevents ),
		m_netevents(*this)  {

	g_rxgserv = this;
}

//-----------------------------------------------------------------------------
RXGServ::~RXGServ() {
	m_netevents.Disable();
}


//-----------------------------------------------------------------------------
void RXGServ::OnStart() {

	try {
		YAML::Node config = YAML::LoadFile("rxgserv.yaml");
		try {
			m_password = config["password"].as<std::string>();
		} catch( std::exception & ) {
			throw std::exception( "`password` is not defined." );
		}
	} catch( std::exception &e ) {
		System::Console::Print( "Error loading config:" );
		System::Console::Print( "%s", e.what() );
		System::Console::Print( "Quitting..." );
		std::this_thread::sleep_for( std::chrono::seconds(5) );
		System::Shutdown();
		return;
	}

	m_listener.Start();
	System::Console::Print( "Ready." );
}


//-----------------------------------------------------------------------------
void NetHandler::Accepted( Net::StreamPtr &stream ) {
	System::Log( "A client connected: address \"%s\"", 
		stream->GetHostname().c_str() );
}

//-----------------------------------------------------------------------------
void NetHandler::AcceptError( 
		Net::StreamPtr &stream,
		const boost::system::error_code &error ) {
	System::Log( "Failed accepting client: %s", error.message().c_str() );
}

//-----------------------------------------------------------------------------
void NetHandler::Disconnected( 
		Net::StreamPtr &stream,
		const boost::system::error_code &error ) {
	
	System::Log( "A client disconnected: address \"%s\"", 
		stream->GetHostname().c_str() );
}

//-----------------------------------------------------------------------------
void NetHandler::SendFailed( 
		Net::StreamPtr &stream,
		const boost::system::error_code &error ) {

	System::Log( "Connection failure: (%s) %s", 
			stream->GetHostname().c_str(), 
			error.message().c_str() );
}

//-----------------------------------------------------------------------------
void NetHandler::Receive( Net::StreamPtr &netstream, 
				Net::Message &netmsg ) {

	auto &stream = netstream->Cast<Stream>();
	auto &msg = netmsg.Cast<Net::TextStream::Message>();

	stream.RunProc( msg() );
}

//-----------------------------------------------------------------------------
NetHandler::NetHandler( RXGServ &parent ) : m_parent(parent) {}
	
}}
