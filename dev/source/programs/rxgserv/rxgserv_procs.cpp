
#include "stdafx.h"
#include "rxgserv.h"

namespace User { namespace RXGServ {
	
//-------------------------------------------------------------------------
class ProcMap {
		
public:
	using ProcType = void (ProcHandler::*)( ProcContext::ptr& ); 
	ProcMap();
	ProcType Get( const std::string &name );
private:

	std::unordered_map< std::string, ProcType > m_map;
} g_procmap;

//-----------------------------------------------------------------------------
ProcMap::ProcMap() {
	m_map[ "TEST" ] = &ProcHandler::Test;
}

//-----------------------------------------------------------------------------
auto ProcMap::Get( const std::string &name ) -> ProcType {
	std::string uppercase = name;
	boost::to_upper( uppercase );
	return m_map.at( uppercase );
}

 
//-----------------------------------------------------------------------------
void ProcHandler::Run( ProcContext::ptr &context ) {
	ProcMap::ProcType proc;
	try { 
		proc = g_procmap.Get( context->Args()[0] );
	} catch( std::out_of_range & ) {
		//  TODO respond with error.
		proc = &ProcHandler::Unknown; 
	}
	
	(*this.*proc)( context );
}


ProcContext::ProcContext( std::shared_ptr<Stream> &stream, 
						  const std::string &command ) : 
		m_stream(stream), m_args( command ) {

}

ProcContext::~ProcContext() {
	Complete();
}

void ProcContext::Complete() {
	if( m_completed ) return;
	m_completed = true;
	m_stream->NextProc();
}

}}
