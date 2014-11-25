
#include "stdafx.h"
#include "rxgserv.h"
#include "responses.h"

using namespace std;

namespace User { namespace RXGServ { namespace Procs {

//-----------------------------------------------------------------------------
namespace {
	std::string Uppercase( const string &a ) {
		string b = a;
		boost::to_upper( b );
		return b;
	}
}
	  
//-----------------------------------------------------------------------------
shared_ptr<Proc> &Map::Get( const string &command ) {
	std::string upper = Uppercase( command );
	if( m_map.count( upper ) == 0 ) {
		return m_map.at( string("") );
	}
	return m_map.at( upper );
}

//-----------------------------------------------------------------------------
void Map::Add( shared_ptr<Proc> &proc ) {
	m_map[ Uppercase( proc->Command() ) ] = proc;
}

//-----------------------------------------------------------------------------
void Map::Run( shared_ptr<Context> &ct ) { 
	auto &proc = Map::Get( ct->Args()[0] );
	if( (ct->Args().Count()-1) < proc->RequiredArgs()  ) {
		ErrorResponse( "FAILED", Util::Format( 
			"This request needs at least %d argument%s.", 
			proc->RequiredArgs(),
			proc->RequiredArgs() == 1 ? "" : "s" )).Write(*ct);
		return;
	}
	(*proc)( ct );
}

//-----------------------------------------------------------------------------
Context::Context( shared_ptr<Stream> &stream, 
				  const string &commandline ) : 
		m_stream(stream), m_args( commandline ) {
}

//-----------------------------------------------------------------------------
Context::~Context() {
	Complete();
}

//-----------------------------------------------------------------------------
void Context::Complete() {
	if( m_completed ) return;
	if( !m_responded ) {
		ErrorResponse( "FAILED", "Invalid usage or undefined error." )
			.Write(*this);
	}
	m_completed = true;
	m_stream->NextProc();
}

}}}
