//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright � 2015, Mukunda Johnson, All rights reserved. =========//

#include "stdafx.h"
#include "object.h"
#include "event.h"

//-----------------------------------------------------------------------------
namespace Ui {

//-----------------------------------------------------------------------------
Object::Object( const Stref &name ) : m_name( name ) {
}

//-----------------------------------------------------------------------------
Object::~Object() {}

//-----------------------------------------------------------------------------
void Object::SendEvent( Event::Event &e ) {

	OnEvent( e );

	// (pass to scripts)
}

//-----------------------------------------------------------------------------
void Object::OnEvent( Event::Event &e ) {}

}
