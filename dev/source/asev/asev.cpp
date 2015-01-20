//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

#include "stdafx.h"
#include "asev.h"

using std::lock_guard;
using std::mutex;
using std::recursive_mutex;

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
	/// Remove matching items from a vector. (once)
	///
	/// @param list std::vector to modify.
	/// @param item Element to look for.
	/// @returns    true if an element was removed.
	///
	template <typename T, typename P>
	bool RemovePointer( std::vector<T> list, P item ) {
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
}

//-----------------------------------------------------------------------------
Handler::~Handler() {  
	assert( m_sourcecount == 0 );
}
 
//-----------------------------------------------------------------------------
void Handler::IncrementSources() {
	lock_guard<recursive_mutex> lock( m_mutex );
	m_sourcecount++;
}

//-----------------------------------------------------------------------------
void Handler::DecrementSources() {
	lock_guard<recursive_mutex> lock( m_mutex );
	m_sourcecount--;
	m_on_unsub.notify_all(); 
}

//-----------------------------------------------------------------------------
Source::HandlerRef::HandlerRef( T &ptr ) {
	m_ptr = ptr;
	if( m_ptr ) m_ptr->IncrementSources();
}

//-----------------------------------------------------------------------------
Source::HandlerRef::HandlerRef( HandlerRef &&other ) {
	// move constructor
	
	// if we already have a pointer (ie this is not a new instance), then
	// clean it up first
	if( m_ptr ) m_ptr->DecrementSources();

	// then steal the pointer from the other instance
	m_ptr = std::move(other.m_ptr);
}

//-----------------------------------------------------------------------------
auto Source::HandlerRef::operator=( HandlerRef &&other ) -> HandlerRef& {
	// alias of move constructor
	if( m_ptr ) m_ptr->DecrementSources();
	m_ptr = std::move(other.m_ptr);
	return *this;
}

//-----------------------------------------------------------------------------
Source::HandlerRef::~HandlerRef() {
	if( m_ptr ) m_ptr->DecrementSources();
}

//-----------------------------------------------------------------------------
Source::Source() {}
Source::~Source() {}

//-----------------------------------------------------------------------------
void Source::AsevSubscribe( Handler::ptr &handler ) {
	if( !handler ) return;
	lock_guard<recursive_mutex> lock( m_mutex );
	m_newhandlers.push_back( handler ); 
	ModifyPipes();
}

//-----------------------------------------------------------------------------
void Source::AsevUnsubscribe( Handler::ptr &handler ) {
	AsevUnsubscribe( handler.get() );
}

//-----------------------------------------------------------------------------
void Source::AsevUnsubscribe( Handler &handler ) {
	AsevUnsubscribe( &handler ); 
}

//-----------------------------------------------------------------------------
void Source::AsevUnsubscribe( Handler *handler ) {
	if( !handler ) return;
	lock_guard<recursive_mutex> lock( m_mutex );
	m_removehandlers.push_back( handler );
	ModifyPipes(); 
}

//-----------------------------------------------------------------------------
void Source::ModifyPipes() {
	// we are locked from outside
	// if we are inside an event handler, defer this call until after
	// the handlers are executed
	if( m_handler_is_executing ) return; // defer!

	// otherwise, we update the handlers immediately

	for( auto &handler : m_newhandlers ) { 
		m_handlers.push_back( std::move( handler ));
	}

	m_newhandlers.clear();

	for( auto &handler : m_removehandlers ) { 
		for( auto &i = m_handlers.begin(); i != m_handlers.end(); i++ ) {

			// iterator -> handler ref -> smart ptr -> raw ptr
			if( (**i).get() == handler ) {
				m_handlers.erase(i);
				break;
			}
		} 
	}

	m_removehandlers.clear();
}

//-----------------------------------------------------------------------------
void Source::AsevDisable() {
	lock_guard<recursive_mutex> lock( m_mutex );
	m_disabled = true;
}

//-----------------------------------------------------------------------------
void Source::AsevEnable() {
	lock_guard<recursive_mutex> lock( m_mutex );
	m_disabled = false;
}

//-----------------------------------------------------------------------------
int Source::SendEvent( Event &e ) {
	int result = 0; 

	if( m_disabled ) return 0;

	m_handler_is_executing = true;
	for( auto &handler : m_handlers ) {

		// lock handler and execute.
		lock_guard<recursive_mutex> lock( (*handler)->GetMutex() );
		result = (*handler)->Handle( e ); 
	}
	m_handler_is_executing = false;
	ModifyPipes(); // flush newpipes and removepipes
	
	return result;
}

//-----------------------------------------------------------------------------
Dispatcher::Dispatcher( Source &source ) :
	m_source( source ), 
	m_lock( m_source.m_mutex )
{
}

//-----------------------------------------------------------------------------
int Dispatcher::Send( Event &e ) {
	return m_source.SendEvent( e );
}

//-----------------------------------------------------------------------------
}
