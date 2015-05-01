//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright � 2015, Mukunda Johnson, All rights reserved. =========//

#include "stdafx.h"
#include "object.h"

//-----------------------------------------------------------------------------
namespace Ui {

//-----------------------------------------------------------------------------
Object::Object( const Stref &name ) : m_name( name ) {
}

//-----------------------------------------------------------------------------
Object::~Object() {}

//-----------------------------------------------------------------------------
void Object::OnEvent( const Event &e ) {
}

}