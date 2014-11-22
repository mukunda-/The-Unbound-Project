
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
		//proc = std::bind( m_procmap.at(args.args[0]), this );// m_map.at(args.args[0]);
		proc = g_procmap.Get( context->Args()[0] );
	} catch( std::out_of_range & ) {
		//  TODO respond with error.
		proc = &ProcHandler::Unknown;
		return;
	}
	
	(*this.*proc)( context );
}

//-----------------------------------------------------------------------------
ProcQueue::ProcQueue( ProcHandler &handler ) : m_handler(handler) {}

//-----------------------------------------------------------------------------
void ProcQueue::Run( const std::string &command ) {

	{
		std::lock_guard<std::mutex> lock( m_mutex );
		m_queue.push_back( command );
		if( m_queue.size() != 1 ) {
			// a command is executing already.
			return;
		}
	}

	Exec( command );
}

//-----------------------------------------------------------------------------
void ProcQueue::Exec( const std::string &cmd ) { 
	Util::ArgString args( cmd );
	m_handler.Run( args ); 
}

//-----------------------------------------------------------------------------
void ProcQueue::Next() {
	const std::string *cmdstr;
	{
		std::lock_guard<std::mutex> lock( m_mutex );
		m_queue.pop_front();
		if( m_queue.empty() ) return;
		cmdstr = &m_queue.front();
	}
	
	Exec( *cmdstr );
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
	m_stream->m_procq.Next();
}

}}
