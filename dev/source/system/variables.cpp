//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "system.h"
#include "variables.h"
#include "util/trie.h"

using namespace std;
using Util::StringRef;

//---------------------------------------------------------------------------------------
namespace System {

extern Main *g_main;
  
//---------------------------------------------------------------------------------------
Variable &Main::CreateVariable( const StringRef &name, 
									const StringRef &default_value,
									const StringRef &description, int flags ) {

	try {
		return *m_variables.at( name ); 
	} catch( std::out_of_range& ) {}
	 
	auto var = new Variable( name, description, default_value, flags );

	m_variables[ name ] = Variable::ptr(var);

	return *var;
}

//-----------------------------------------------------------------------------
bool Main::DeleteVariable( const StringRef &name ) { 

	try {
		m_variables.erase( name );
		return true;
	} catch( std::out_of_range& ) {}

	return false;
}
 
//-----------------------------------------------------------------------------
Variable *Main::FindVariable( const StringRef &name ) {
	try {
		return m_variables.at( name ).get();
	} catch( std::out_of_range& ) {}
	return nullptr;
}

//-----------------------------------------------------------------------------
namespace {

	std::string FormatFloat( double d ) {
		std::string str = std::to_string(d);
		str.erase( str.find_last_not_of('0') + 1, std::string::npos );
		if( str.back() == '.' ) str += '0';
		return str;
	}
}

//-----------------------------------------------------------------------------
namespace Variables {

	//-------------------------------------------------------------------------
	Variable &Create( const StringRef &name, const StringRef &default_value,
					  const StringRef &description, int flags ) {

		return g_main->CreateVariable( 
				name, default_value, description, flags );
	}

	//-------------------------------------------------------------------------
	bool Delete( const StringRef &name ) {
		return g_main->DeleteVariable( name );
	}

	//-------------------------------------------------------------------------
	Variable *Find( const StringRef &name ) {
		return g_main->FindVariable( name );
	}
}

//-----------------------------------------------------------------------------
void Variable::PrintInfo() {
	::Console::Print( "%s: \"%s\" | %s", m_name, GetCString(), m_description );
}

//-----------------------------------------------------------------------------
Variable::Variable( const StringRef &name, 
			const StringRef &description,
			const StringRef &init,
			int flags ) {

	m_name = name;
	m_description = description;
	m_flags = flags;
	SetStringI( init, true );
}

//-----------------------------------------------------------------------------
Variable::~Variable() {}

//-----------------------------------------------------------------------------
bool Variable::SetInt( int value ) {
	m_prev = m_value;

	m_value.m_int = value;
	m_value.m_float = (double)value;
	m_value.m_string = std::to_string( value );

	return OnChanged();
}

//-----------------------------------------------------------------------------
bool Variable::SetFloat( double value ) {
	m_prev = m_value;

	m_value.m_int = (int)floor(value);
	m_value.m_float = value;
	m_value.m_string = FormatFloat( value );

	return OnChanged();
}

//-----------------------------------------------------------------------------
bool Variable::SetStringI( const StringRef &value, bool skipchg ) {
	m_prev = m_value;

	try {
		m_value.m_int = 0;
		m_value.m_int = std::stoi( value );
	} catch( std::invalid_argument& ) {
	} catch( std::out_of_range& ) {}
	
	try {
		m_value.m_float = 0;
		m_value.m_float = std::stod( value );
	} catch( std::invalid_argument& ) {
	} catch( std::out_of_range& ) {}

	m_value.m_string = value.Copy();

	return skipchg ? false : OnChanged();
}

//-----------------------------------------------------------------------------
bool Variable::SetString( const StringRef &value ) {
	return SetStringI( value, false );
}

//-----------------------------------------------------------------------------
int Variable::HookChange( ChangeHandler callback ) {
	int id = m_changehandler_nextid++;
	m_change_handlers.push_back( std::pair<int,ChangeHandler>( id, callback ));
	return id;
}

//-----------------------------------------------------------------------------
void Variable::UnhookChange( int id ) {
	for( auto handler = m_change_handlers.begin(); 
			  handler != m_change_handlers.end(); handler++ ) {

		if( (*handler).first == id ) {
			m_change_handlers.erase( handler );
			return;
		}
	}

	throw std::runtime_error( 
		"Tried to unhook nonexistant sysvar change handler." );
}

//-----------------------------------------------------------------------------
bool Variable::OnChanged() {
	if( m_value == m_prev ) return false;
	for( auto &handler : m_change_handlers ) {
		handler.second( *this );
	}
	return true;
}

}
