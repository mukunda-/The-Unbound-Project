#include "stdafx.h"
#include "rxgserv.h" 

namespace User { namespace RXGServ {

	//-----------------------------------------------------------------------------
	Stream::Stream( RXGServ &serv) : m_serv( serv ) {}
	 
	//-----------------------------------------------------------------------------
	void Stream::RunProc( const std::string &command ) {

		{
			std::lock_guard<std::mutex> lock( m_mutex );
			m_procqueue.push_back( command );
			if( m_procqueue.size() != 1 ) {
				// a command is executing already.
				return;
			}
		}

		ExecProc( command );
	}

	//-----------------------------------------------------------------------------
	void Stream::ExecProc( const std::string &cmd ) { 

		auto ctx = std::make_shared<Procs::Context>( 
			std::static_pointer_cast<Stream>( shared_from_this() ), cmd );
		m_serv.RunProc( ctx );
	}

	//-----------------------------------------------------------------------------
	void Stream::NextProc() {
		const std::string *cmdstr;
		{
			std::lock_guard<std::mutex> lock( m_mutex );
			m_procqueue.pop_front();
			if( m_procqueue.empty() ) return;
			cmdstr = &m_procqueue.front();
		}
	
		ExecProc( *cmdstr );
	}
}}
