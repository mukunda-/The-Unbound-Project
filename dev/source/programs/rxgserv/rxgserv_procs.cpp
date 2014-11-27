
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

	bool needargs = true;

	if( (ct->Args().Count()-1) >= proc->RequiredArgs() ) {
		needargs = false;
		for( int i = 0; i < proc->RequiredArgs(); i++ ) {
			if( ct->Args()[1+i].size() == 0 ) {
				needargs = true;
				break;
			}
		}
	}
	
	if( needargs ) {
		ErrorResponse( "FAILED", Util::Format( 
			"This request needs at least %d argument%s.", 
			proc->RequiredArgs(),
			proc->RequiredArgs() == 1 ? "" : "s" )).Write(*ct);
		return;
	}
	(*proc)( ct );
}

void Proc::operator()( Context::ptr &ct ) { 
	if( Locking() ) {
		ct->m_lock = unique_lock<mutex>( m_mutex );
	}
	std::lock_guard<std::mutex> lock( m_mutex );
	Run( ct ); 
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

//-----------------------------------------------------------------------------
void Context::RespondSimple( const std::string &text ) {
	SimpleResponse( text ).Write( *this );
}

//-----------------------------------------------------------------------------
void Context::RespondError( const std::string &status, 
							const std::string &desc ) {

	ErrorResponse( status, desc ).Write( *this );
}

//-----------------------------------------------------------------------------
void Context::RespondDBError() {
	ErrorResponse( "DBFAILURE", "A database error occurred." ).Write( *this );
}


}}}
