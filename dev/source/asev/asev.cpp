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
	/// @returns true if the vector contains the element.
	///
	template <typename T>
	bool ListContains( std::vector<T> list, T item ) {
		for( Handler *h : m_handlers ) {
			if( h == &handler ) return true;
		}
		return false;
	}
	
	/// -----------------------------------------------------------------------
	/// Remove an element from a vector. (once)
	///
	/// @param list std::vector to modify.
	/// @param item Element to look for.
	/// @returns true if an element was removed.
	///
	template <typename T>
	bool RemoveFromList( std::vector<T> list, T item ) {
		for( auto i = m_handlers.begin(); i != m_handlers.end(); i++ ) {
			if( *i == &handler ) {
				m_handlers.erase(i);
				return true;
			}
		}
		return false;
	}
}

//-----------------------------------------------------------------------------
Handler::Handler() {
	m_disabled = false;
}

//-----------------------------------------------------------------------------
Handler::~Handler() {
	if( !m_disabled ) {
		throw std::runtime_error( "Attempt to destruct live event handler." );
	}
}

//-----------------------------------------------------------------------------
void Handler::Disable() {
	std::lock_guard<std::mutex> lock( m_mutex );
	m_disabled = true;
}

//-----------------------------------------------------------------------------
void Source::Subscribe( Handler &handler ) {
	std::lock_guard<std::mutex> lock( m_mutex );
	std::lock_guard<std::mutex> lock2( handler.pipe->GetLock() );
	
	if( ListContains( m_pipes, handler.pipe ) ) return; 
	m_pipes.push_back( handler.pipe ); 
}

//-----------------------------------------------------------------------------
void Source::Unsubscribe( Handler &handler ) {
	std::lock_guard<std::mutex> lock( m_mutex );
	std::lock_guard<std::mutex> lock2( handler.pipe->GetLock() );

	RemoveFromList( m_pipes, handler.pipe );
}

//-----------------------------------------------------------------------------
Dispatcher::Dispatcher( Source &source ) :
	m_source(source), 
	m_lock( m_source.m_mutex )
{

}

//-----------------------------------------------------------------------------
Dispatcher::Send( Event &e ) {
	for( auto pipe = m_source.m_pipes.begin(); 
			pipe != m_source.m_pipes.end(); pipe++ ) {

		

		std::lock_guard<std::mutex> lock( handler.m_mutex );
		if( handler.m_disabled ) continue;
		handler.Handle( e );
	}
}

//-----------------------------------------------------------------------------
}
