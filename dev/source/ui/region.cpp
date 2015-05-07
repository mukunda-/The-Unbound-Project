//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2015, Mukunda Johnson, All rights reserved. =========//

#include "stdafx.h"

#include "region.h"
#include "ui.h"

//-----------------------------------------------------------------------------
namespace Ui {
	
extern Instance *g_ui;

//-----------------------------------------------------------------------------
Region::Region( const Stref &name ) : Object( name ) {

	// register with ui
	g_ui->OnRegionCreated( *this );
}

//-----------------------------------------------------------------------------
Region::~Region() {

	// remove all references.
	ClearParent();
	for( auto r : m_children ) {
		r->ClearParent();
	}
	m_anchor_list.clear();
	
	ClearAnchor();
	for( auto r : m_anchor_list ) {
		r->ClearAnchor();
	}
	m_anchor_list.clear();

	// unregister
	g_ui->OnRegionDeleted( *this );
}

//-----------------------------------------------------------------------------
void Region::SetPoints( Anchor h_from, Anchor h_to, 
	                    Anchor v_from, Anchor v_to,
						int h_offset, int v_offset ) {

}

//-----------------------------------------------------------------------------
void Region::SetHorizontalPoint( Anchor from, Anchor to, int offset ) {

}

void Region::SetVerticalPoint( Anchor from, Anchor to, int offset ) {

}

//-----------------------------------------------------------------------------
}