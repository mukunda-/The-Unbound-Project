
#include "stdafx.h"
#include "rxgserv.h"
#include "system/console.h"
#include "system/commands.h"
#include "util/stringles.h"

#include "db/core.h"
#include "db/endpoint.h"

//-----------------------------------------------------------------------------
namespace User { namespace RXGServ {

RXGServ *g_rxgserv;


//-----------------------------------------------------------------------------
RXGServ::RXGServ() :
		m_listener( LISTEN_PORT, 
			std::bind( &RXGServ::StreamFactory,this), &m_netevents ),

		m_netevents(*this) {

	g_rxgserv = this;

	// add procs here
	 
	m_pmap.Add<MyProcs::Unknown>();
	m_pmap.Add<MyProcs::Test>();
	m_pmap.Add<MyProcs::Auth>();
	m_pmap.Add<MyProcs::Perks>();
	m_pmap.Add<MyProcs::Member>();
	m_pmap.Add<MyProcs::Donations>();
}

//-----------------------------------------------------------------------------
RXGServ::~RXGServ() {
	m_netevents.Disable();
}


//-----------------------------------------------------------------------------
void RXGServ::OnStart() {

	using std::string;
	using Util::Format;

	DB::Endpoint ep_forums;
	try {
		YAML::Node config = YAML::LoadFile("rxgserv.yaml");

		string var;
		try {
			var = "password";
			m_password = config["password"].as<string>();

			const YAML::Node &node = config["sql"]["forums"];
			
			var = "sql.forums.address";
			ep_forums.m_address  = node["address"].as<string>();

			var = "sql.forums.username";
			ep_forums.m_username = node["username"].as<string>();

			var = "sql.forums.password";
			ep_forums.m_password = node["password"].as<string>();

			var = "sql.forums.database";
			ep_forums.m_database = node["database"].as<string>();
			
		} catch( YAML::RepresentationException & ) { 
			throw std::runtime_error( 
				Format( "missing required field: %s", var ));
		}
	} catch( std::runtime_error &e ) {
		System::Console::Print( "Error loading config:" );
		System::Console::Print( "%s", e.what() );
		System::Console::Print( "Quitting..." );
		std::this_thread::sleep_for( std::chrono::seconds(5) );
		System::Shutdown();
		return;
	}

	DB::Register( "FORUMS", ep_forums );

	m_listener.Start();
	System::Console::Print( "Ready." );
}

//-----------------------------------------------------------------------------
void RXGServ::RunProc( std::shared_ptr<Procs::Context> &ct ) {
	m_pmap.Run( ct );
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
