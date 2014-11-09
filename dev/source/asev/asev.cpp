//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

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
		for( Handler *h : list ) {
			if( h == &item ) return true;
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
			if( *i == &item ) {
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
void Source::Subscribe( Handler &handler ) {
	std::lock_guard<std::mutex> lock( m_mutex );
	std::lock_guard<std::mutex> lock2( handler.m_pipe->GetLock() );
	
	if( ListContains( m_pipes, handler.m_pipe ) ) return; 
	m_pipes.push_back( handler.m_pipe ); 
}

//-----------------------------------------------------------------------------
void Source::Unsubscribe( Handler &handler ) {
	std::lock_guard<std::mutex> lock( m_mutex );
	std::lock_guard<std::mutex> lock2( handler.m_pipe->GetLock() );

	RemoveFromList( m_pipes, handler.m_pipe );
}

//-----------------------------------------------------------------------------
Dispatcher::Dispatcher( Source &source ) :
	m_source(source), 
	m_lock( m_source.m_mutex )
{

}

//-----------------------------------------------------------------------------
void Dispatcher::Send( Event &e ) {
	for( auto pipe = m_source.m_pipes.begin(); 
			pipe != m_source.m_pipes.end(); pipe++ ) {

		Handler::Lock lock( *pipe );
		if( lock() == nullptr ) {
			m_source.m_pipes.erase(
		}

		std::lock_guard<std::mutex> lock( handler.m_mutex );
		if( handler.m_disabled ) continue;
		handler.Handle( e );
	}
}

//-----------------------------------------------------------------------------
}
