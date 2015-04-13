//===========================  The Unbound Project  =========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

#include <stdafx.h>
#include "console.h"

using namespace std;

namespace Console {
//-----------------------------------------------------------------------------

Instance *g_instance;

//-----------------------------------------------------------------------------
void Print( const Util::StringRef &text ) {
	assert( g_instance );
	g_instance->Print( text.CStr(), true, false );
}
 
//-----------------------------------------------------------------------------
void PrintErr( const Util::StringRef &text ) {
	assert( g_instance );
	g_instance->Print( text.CStr(), true, true );
}
 
//-----------------------------------------------------------------------------
void PrintEx( const Util::StringRef &text ) {
	assert( g_instance );
	g_instance->Print( text.CStr(), false, false );
} 

//-----------------------------------------------------------------------------
void PrintErrEx( const Util::StringRef &text ) {
	assert( g_instance );
	g_instance->Print( text.CStr(), false, true );
}

//-----------------------------------------------------------------------------
void SetPrintHandler( PrintHandler::ptr handler ) {
	assert( g_instance );
	g_instance->SetPrintHandler( handler );
} 

//-----------------------------------------------------------------------------
Instance::Instance() {
	
	if( g_instance != nullptr ) {
		throw std::runtime_error( 
			"Cannot create multiple console instances." );
	}
	g_instance = this;
	m_printhandler = make_shared<PrintHandler>();
}

//-----------------------------------------------------------------------------
Instance::~Instance() {
	g_instance = nullptr;
}

//-----------------------------------------------------------------------------
void Instance::Print( const char *text, bool newline, bool error ) {
	if( newline ) {
		string copy(text);
		copy += "\n";
		
		{
			lock_guard<mutex> lock( m_print_mutex );
			m_printhandler->Print( copy.c_str(), error );
		}
	} else {
		lock_guard<mutex> lock( m_print_mutex );
		m_printhandler->Print( text, error );
	}
}

//-----------------------------------------------------------------------------
void Instance::SetPrintHandler( PrintHandler::ptr handler ) {
	lock_guard<mutex> lock(m_print_mutex);
	m_printhandler = handler;
}

//-----------------------------------------------------------------------------
void PrintHandler::Print( const char *text, bool error ) {
	if( !error ) {
		std::cout << text;
	} else {
		std::cerr << text;
	}
} 

//-----------------------------------------------------------------------------
}
