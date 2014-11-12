//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright � 2014, Mukunda Johnson, All rights reserved. =========//

#include "stdafx.h"
#include "asev.h"

namespace Asev {

namespace {
	
	/// -----------------------------------------------------------------------
	/// Check if a vector contains an element.
	///
	/// @param list Vector to test.
	/// @param item Element to look for.
	/// @returns    true if the vector contains the element.
	///
	template <typename T>
	bool ListContains( std::vector<T> list, T item ) {
		for( auto &h : list ) {
			if( h == item ) return true;
		}
		return false;
	}
	
	/// -----------------------------------------------------------------------
	/// Remove an element from a vector. (once)
	///
	/// @param list std::vector to modify.
	/// @param item Element to look for.
	/// @returns    true if an element was removed.
	///
	template <typename T>
	bool RemoveFromList( std::vector<T> list, T item ) {
		for( auto i = list.begin(); i != list.end(); i++ ) {
			if( *i == item ) {
				list.erase(i);
				return true;
			}
		}
		return false;
	}
}

//-----------------------------------------------------------------------------
Handler::Handler() {
	m_pipe = std::make_shared<Pipe>( *this );
}

//-----------------------------------------------------------------------------
Handler::~Handler() {
	if( !Lock(*m_pipe).IsClosed() ) {
		throw std::runtime_error( "Attempt to destruct live event handler." );
	}
}

//-----------------------------------------------------------------------------
void Handler::Disable() {
	Lock(*m_pipe).Close();
}

//-----------------------------------------------------------------------------
Handler::Pipe::Pipe( Handler &parent ) {
	m_handler = &parent;
}

//-----------------------------------------------------------------------------
Handler::Lock::Lock( Pipe &pipe ) : m_pipe(pipe), m_lock(pipe.m_mutex) {
	
}

//-----------------------------------------------------------------------------
Handler *Handler::Lock::operator()() {
	return m_pipe.m_handler;
}

//-----------------------------------------------------------------------------
void Handler::Lock::Close() {
	m_pipe.m_handler = nullptr;
}

//-----------------------------------------------------------------------------
bool Handler::Lock::IsClosed() {
	return m_pipe.m_handler == nullptr;
}

//-----------------------------------------------------------------------------
Source::Source() {
}

//-----------------------------------------------------------------------------
void Source::AsevSubscribe( Handler &handler ) {
	std::lock_guard<std::mutex> lock( m_mutex );
	std::lock_guard<std::mutex> lock2( handler.m_pipe->GetLock() );
	
	if( ListContains( m_pipes, handler.m_pipe ) ) return; 
	m_pipes.push_back( handler.m_pipe ); 
}

//-----------------------------------------------------------------------------
void Source::AsevUnsubscribe( Handler &handler ) {
	std::lock_guard<std::mutex> lock( m_mutex );
	std::lock_guard<std::mutex> lock2( handler.m_pipe->GetLock() );

	RemoveFromList( m_pipes, handler.m_pipe );
}

//-----------------------------------------------------------------------------
void Source::AsevDisable() {
	std::lock_guard<std::mutex> lock( m_mutex );
	m_disabled = true;
}

//-----------------------------------------------------------------------------
void Source::AsevEnable() {
	std::lock_guard<std::mutex> lock( m_mutex );
	m_disabled = false;
}

//-----------------------------------------------------------------------------
Dispatcher::Dispatcher( Source &source ) :
	m_source( source ), 
	m_lock( m_source.m_mutex )
{

}

//-----------------------------------------------------------------------------
int Dispatcher::Send( Event &e ) {
	int result = 0; 

	if( m_source.m_disabled ) return 0;

	for( auto pipe = m_source.m_pipes.begin(); 
			pipe != m_source.m_pipes.end(); ) {

		Handler *handler;
		{
			Handler::Lock lock( **pipe );
			handler = lock();
			if( handler ) {
				result = handler->Handle( e );
				pipe++;
			}
		}

		// this pipe is closed. remove it.
		pipe = m_source.m_pipes.erase( pipe );
	}
	return result;
}

//-----------------------------------------------------------------------------
}
