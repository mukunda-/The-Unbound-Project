//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

#pragma once

namespace Net {

	/// -----------------------------------------------------------------------
	/// A simple interface to move the sendlock and open a stream.
	///
	class WriterBase {

	protected:
		Stream::SendLock m_sendlock;
		std::ostream m_stream;  

		WriterBase( Stream::SendLock &&lock ) :
				m_sendlock( std::move(lock) ), 
				m_stream( &m_sendlock.Buffer() ) { 
		} 
	};

}