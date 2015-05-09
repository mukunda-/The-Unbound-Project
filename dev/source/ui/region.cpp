//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2015, Mukunda Johnson, All rights reserved. =========//

#include "stdafx.h"

#include "region.h"
#include "ui.h"
#include "console/console.h"

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
void Region::SetPointsThing( Anchor from, Anchor to, int index, 
		                 bool set_offset, int offset, 
						 bool set_percent, float percent ) {

	if( from == Anchor::MIDDLE ) {
		
		// unset edges
		m_points[index].Unset();
		m_points[index+2].Unset();
		
		// set middle
		if( set_offset  ) m_points[index+1].Set( offset );
		if( set_percent ) m_points[index+1].SetPercent( percent );

		m_anchor_points[index+1] = to;

	} else if( from == Anchor::LEFT ) {

		m_points[index+1].Unset(); // unset middle
		
		// set left/top
		if( set_offset  ) m_points[index].Set( offset );
		if( set_percent ) m_points[index].SetPercent( percent );

		m_anchor_points[index] = to;

	} else if( from == Anchor::RIGHT ) {

		m_points[index+1].Unset(); // unset middle
		
		// set right/bottom
		if( set_offset  ) m_points[index+2].Set( offset );
		if( set_percent ) m_points[index+2].SetPercent( percent );

		m_anchor_points[index+2] = to;
	}

	ComputeRect();
}

//-----------------------------------------------------------------------------
void Region::SetPoints( Anchor h_from, Anchor h_to, 
	                    Anchor v_from, Anchor v_to,
						int h_offset, int v_offset ) {

	SetHorizontalPoint( h_from, h_to, h_offset );
	SetVerticalPoint  ( v_from, v_to, v_offset );
}

//-----------------------------------------------------------------------------
void Region::SetHorizontalPoint( Anchor from, Anchor to, int offset ) {

	SetPointsThing( from, to, 0, true, offset, false, 0.0 );
}

//-----------------------------------------------------------------------------
void Region::SetVerticalPoint( Anchor from, Anchor to, int offset ) {

	SetPointsThing( from, to, 3, true, offset, false, 0.0 );
}

//-----------------------------------------------------------------------------
void Region::SetPointsPercent( Anchor h_from, Anchor h_to, 
	                           Anchor v_from, Anchor v_to,
							   float h_offset, float v_offset ) {

	SetHorizontalPointPercent( h_from, h_to, h_offset );
	SetVerticalPointPercent  ( v_from, v_to, v_offset );
}

//-----------------------------------------------------------------------------
void Region::SetHorizontalPointPercent( Anchor from, Anchor to,
	                                    float offset ) {

	SetPointsThing( from, to, 0, false, 0, true, offset );
}

//-----------------------------------------------------------------------------
void Region::SetVerticalPointPercent( Anchor from, Anchor to,
	                                  float offset ) {
	SetPointsThing( from, to, 3, false, 0, true, offset );
}

//-----------------------------------------------------------------------------
void Region::ComputeWith( Region &anchor ) {

	if( !anchor.m_computed_valid ) {
		// anchor isn't computed.
		return;
	}

	if( m_points[0].set && m_points[2].set ) {
		// left and right set

		if( m_anchor_points[0] ) {

		}
		m_computed_rect[0] = anchor.m_computed_rect[0] + m_points[0].offset + 
	}
}

//-----------------------------------------------------------------------------
void Region::ComputeRect() {

	if( g_ui->m_computing_region == this ) {

		Console::PrintErr( 
			"Circular anchor for \"%s\".", 
			g_ui->m_computing_region->GetName() );

		return;
	}

	if( g_ui->m_computing_region == nullptr ) {
		g_ui->m_computing_region = this;
	}


	m_computed_valid = false;

	Region *anchor;
	
	if( m_anchor_to ) {
		anchor = m_anchor_to;
	} else if( m_parent ) {
		anchor = m_parent;
	} else {
		anchor = &g_ui->GetScreen();
	}

	ComputeWith( *anchor );

	// finished.
	g_ui->m_computing_region = nullptr;
}

//-----------------------------------------------------------------------------
}