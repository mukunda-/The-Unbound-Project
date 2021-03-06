//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright � 2015, Mukunda Johnson, All rights reserved. =========//

#include "stdafx.h"
#include "textstream.h"

namespace Net {
	
	//-------------------------------------------------------------------------
	int TextStream::ProcessInput(  int bytes_available ) {
		 
		for( int bytesleft = bytes_available; bytesleft; bytesleft-- ) {
			int c = GetInputStream().get();
			if( c == '\n' ) {
				// execute.
				if( m_current_message.empty() ) continue;
				Message msg( std::move( m_current_message ) );
				
				if( !m_close_called ) {
					Receive( msg );
					Events::Stream::Dispatcher( shared_from_this() )
						.Receive( msg );
				}

				m_current_message.clear();
				continue;
			} else if( c == '\r' ) {
				continue;
			}
			m_current_message += (char)c; 
		}
		return bytes_available;
	}
	
	//-------------------------------------------------------------------------
	TextStream::Writer TextStream::Write() { 
		return Writer( AcquireSendBuffer() ); 
	}
	
	//-------------------------------------------------------------------------
	TextStream::Writer::Writer( Stream::SendLock &&lock ) : 
		WriterBase( std::move(lock) ) {}
}