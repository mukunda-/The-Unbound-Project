//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

#include "stdafx.h"
#include "asev.h"

namespace Asev {

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
	std::lock_guard<std::mutex> lock2( handler.m_mutex );
	
	for( Handler *h : handler.m_sources ) {
		if( h == this ) return;
	}


}

//-----------------------------------------------------------------------------
void Source::Unsubscribe( Handler &handler ) {

}

//-----------------------------------------------------------------------------
Dispatcher::Dispatcher( Source &source ) :
	m_source(source), 
	m_lock( m_source.m_mutex )
{

}

//-----------------------------------------------------------------------------
Dispatcher::Send( Event &e ) {
	for( Handler &handler : m_source.m_handlers ) {
		std::lock_guard<std::mutex> lock( handler.m_mutex );
		if( handler.m_disabled ) continue;
		handler.Handle( e );
	}
}

//-----------------------------------------------------------------------------
}
