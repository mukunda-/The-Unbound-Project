//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "system.h"
#include "variables.h"
#include "util/trie.h"

//---------------------------------------------------------------------------------------
namespace System {

extern Instance *g_instance;

const char *Variable::TYPENAMES[] = {
	"INT",
	"FLOAT",
	"STRING"
};
 
//---------------------------------------------------------------------------------------
Variable &CreateVariable( const char *name, Variable::Type type, 
					  const char *default_value, const char *description, int flags ) {

	Variable *cvar = Find( name );
	if( cvar ) return *cvar;
	
	cvar = new Variable( name, type, description, flags );
	if( default_value ) cvar->SetString( default_value, false );

	references.Set( name, cvar );
	return *cvar;
}

//---------------------------------------------------------------------------------------
Variable *Variable::Find( const char *name ) {
	Variable *cvar;
	if( !references.Get( name, cvar ) ) return nullptr;
	return cvar;
}

//---------------------------------------------------------------------------------------
void Variable::PrintInfo() {
	::Console::Print( "%s [%s]: \"%s\" | %s", 
		m_name, TYPENAMES[m_type], GetString(false).c_str(), m_description.c_str() );
}

}
