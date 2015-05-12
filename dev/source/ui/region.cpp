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
namespace {
	int StrataBase( Strata strata ) {
		assert( strata != Strata::INHERIT );
		return (int)strata * 10000;
	}

	enum {
		STRATA_LEVEL_INCREMENT = 100
	};
}

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
void Region::SetupScreen( int width, int height ) {
	m_computed_rect   = ivec4( 0, 0, width, height );
	m_computed_valid  = true;
	m_computed_strata = StrataBase( Strata::BACKGROUND );
	m_computed_size   = ivec2( width, height );
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
int Region::GetAnchorPos( Region &anchor, Anchor pos, int set ) {
	
	if( pos == Anchor::LEFT ) { // left or top
		return anchor.m_computed_rect[set];
	} else if( pos == Anchor::RIGHT ) { // right or bottom
		return anchor.m_computed_rect[set+2];
	} else { // middle
		return (anchor.m_computed_rect[set]+anchor.m_computed_rect[set+2])/2;
	}
}

//-----------------------------------------------------------------------------
int Region::ComputePoint( Region &anchor, Anchor pos, 
	                      int set, Point point ) {

	return GetAnchorPos( anchor, pos, set ) + point.offset 
		   + point.percent * anchor.m_computed_size[set];
}

//-----------------------------------------------------------------------------
int Region::ComputeSize( Region &anchor, int set ) {
	
	return m_size[set].offset 
		   + m_size[set].percent * anchor.m_computed_size[set];
}

//-----------------------------------------------------------------------------
int Region::ComputeDimension( Region &anchor, int set ) {

	int ip = set * 3; // index base for points and anchor_points
	int ir = set * 2; // index base for ?

	if( m_points[ip].set && m_points[ip+2].set ) {
		// left and right set, compute from points
		 
		m_computed_rect[0+set] = 
				ComputePoint( anchor, m_anchor_points[ip], set, m_points[ip] );

		m_computed_rect[2+set] = 
				ComputePoint( anchor, m_anchor_points[ip+2], set, m_points[ip] );

	} else if( m_points[ip+1].set && m_size[set].set ) {
		// center set, compute from size

		int pos = ComputePoint( anchor, m_anchor_points[ip+1], set, m_points[ip+1] );
		int size = ComputeSize( anchor, set );

		m_computed_rect[0+set] = pos - size / 2;
		m_computed_rect[2+set] = m_computed_rect[0+set] + size;
		
	} else if( m_points[ip].set && m_size[set].set ) {

		m_computed_rect[0+set] = 
				ComputePoint( anchor, m_anchor_points[ip], set, m_points[ip] );

		m_computed_rect[2+set] =
				m_computed_rect[0+set] + ComputeSize( anchor, set );

	} else if( m_points[ip+2].set && m_size[set].set ) {

		m_computed_rect[2+set] =
				ComputePoint( anchor, m_anchor_points[ip], set, m_points[ip] );

		m_computed_rect[0+set] = 
				m_computed_rect[2+set] - ComputeSize( anchor, set );

	} else {
		// not enough data to compute.
		return;
	}

	m_computed_size[set] = m_computed_rect[2+set] - m_computed_rect[0+set];
}

//-----------------------------------------------------------------------------
void Region::ComputeWith( Region &anchor ) {

	if( !anchor.m_computed_valid ) {
		// anchor isn't computed.
		return;
	}

	ComputeDimension( anchor, 0 );
	
	// catch negative width
	if( m_computed_size[0] < 0 ) {
		return;
	}

	ComputeDimension( anchor, 1 );

	if( m_computed_size[1] < 0 ) {
		return;
	}

	m_computed_valid = true;
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

	assert( GetName() != "Screen" ); // screen should never be computed.

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

	// compute strata
	Region *parent = m_parent;
	if( parent == nullptr ) parent = &g_ui->GetScreen();
	
	if( m_strata == Strata::INHERIT ) {
		m_computed_strata = parent->m_computed_strata + STRATA_LEVEL_INCREMENT
			                + m_strata_offset;
	} else {
		m_computed_strata = StrataBase( m_strata ) + m_strata_offset;
	}
	
	// recompute linked regions
	for( auto r : m_anchor_list ) {
		r->ComputeRect();
	}

	// finished.
	g_ui->m_computing_region = nullptr;
}

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
}