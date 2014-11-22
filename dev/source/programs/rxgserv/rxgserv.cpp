
#include "stdafx.h"
#include "rxgserv.h"
#include "system/console.h"
#include "util/stringles.h"

//-----------------------------------------------------------------------------
namespace User { namespace RXGServ {

//-----------------------------------------------------------------------------
RXGServ::RXGServ() :
		m_listener( LISTEN_PORT, StreamFactory, &m_netevents ),
		m_netevents(*this)  {
}

//-----------------------------------------------------------------------------
RXGServ::~RXGServ() {
	m_netevents.Disable();
}

//-----------------------------------------------------------------------------
void RXGServ::OnStart() {
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

//	stream.m_procq.Run( msg() );
}

//-----------------------------------------------------------------------------
NetHandler::NetHandler( RXGServ &parent ) : m_parent(parent) {}
	
}}
