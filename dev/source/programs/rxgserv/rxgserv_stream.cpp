#include "stdafx.h"
#include "rxgserv.h"

namespace User { namespace RXGServ {

	//-----------------------------------------------------------------------------
	Stream::Stream() : m_prochandler( *this ) {}
	 
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
	//	Util::ArgString args( cmd );
		m_prochandler.Run( m_prochandler.CreateContext( cmd ) ); 
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
