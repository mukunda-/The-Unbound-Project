//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2015, Mukunda Johnson, All rights reserved. =========//

#include "stdafx.h"
#include "object.h"
#include "event.h"
#include "ui.h"

//-----------------------------------------------------------------------------
namespace Ui {

extern Instance *g_ui;

//-----------------------------------------------------------------------------
Object::Object( const Stref &name ) : m_name( name ) {
	g_ui->OnObjectCreated( *this );
}

//-----------------------------------------------------------------------------
Object::~Object() {
	g_ui->OnObjectDeleted( *this );
}

//-----------------------------------------------------------------------------
void Object::SendEvent( Event::Event &e ) {

	OnEvent( e );

	// (pass to scripts)
}

//-----------------------------------------------------------------------------
void Object::OnEvent( Event::Event &e ) {}

}
