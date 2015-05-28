//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2015, Mukunda Johnson, All rights reserved. =========//

#include "stdafx.h"
#include "error.h"

#define DEFERR( name, code, text ) name::name() : Error( code, text ) {}

//-----------------------------------------------------------------------------
namespace Ui { namespace Error {
	 
//-----------------------------------------------------------------------------
Error::Error( int code, const Stref &Text ) 
	: std::runtime_error( Text ), m_code(code) {
}

//-----------------------------------------------------------------------------
DEFERR( NameCollision, NAME_COLLISION, "Name Collision." )
DEFERR( BadName, BAD_NAME, "Invalid object name." )

//-----------------------------------------------------------------------------
}}