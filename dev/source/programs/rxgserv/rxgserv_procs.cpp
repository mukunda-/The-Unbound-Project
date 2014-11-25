
#include "stdafx.h"
#include "rxgserv.h"

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
		return m_map.at( 0 );
	}
	return m_map.at( upper );
}

//-----------------------------------------------------------------------------
void Map::Add( shared_ptr<Proc> &proc ) {
	m_map[ Uppercase( proc->Command() ) ] = proc;
}

//-----------------------------------------------------------------------------
void Map::Run( shared_ptr<Context> &ct ) {
	const string &cmdname = ct->Args().Count() == 0 ? "" : ct->Args()[0];
	auto &proc = Map::Get( cmdname );
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
	m_completed = true;
	m_stream->NextProc();
}

}}}
