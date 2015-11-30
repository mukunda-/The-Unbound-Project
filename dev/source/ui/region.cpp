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

	if( GetName() == "Screen" ) {

		// screen is special.
		m_parent = nullptr;

	} else {

		auto &screen = g_ui->GetScreen();

		m_parent           = &screen;
		m_anchor_to        = &screen;
		m_anchor_to_parent = true;
		
		screen.AddChild( *this );
		screen.AddAnchored( *this );
	}
}

//-----------------------------------------------------------------------------
Region::~Region() {

	// remove all references. 
	if( m_parent ) {
		m_parent->RemoveChild( *this );
		for( auto r : m_children ) {
			r->SetParent( nullptr );
		}
		m_children.clear();
	}
	
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

	for( auto r : m_anchor_list ) {
		r->Compute();
	}
}

//-----------------------------------------------------------------------------
/** ---------------------------------------------------------------------------
 * Set points helper function.
 *
 * @param from, to   Anchors to link.
 * @param index      0 for horizontal, 3 for vertical.
 * @param set_offset true to set the pixel offset for the point.
 * @param offset     The pixel offset for the point.
 * @param set_percent true to set the relative offset for the point.
 * @param percent    The relative offset for the point.
 */
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

	Compute();
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
void Region::SetAllPoints( int margin, float margin_p ) {
	SetPointsThing( Anchor::LEFT,   Anchor::LEFT,   0, true, margin, true, margin_p );
	SetPointsThing( Anchor::TOP,    Anchor::TOP,    3, true, margin, true, margin_p );
	SetPointsThing( Anchor::RIGHT,  Anchor::RIGHT,  0, true, margin, true, margin_p );
	SetPointsThing( Anchor::BOTTOM, Anchor::BOTTOM, 3, true, margin, true, margin_p );
	// ...
}

//-----------------------------------------------------------------------------
void Region::SetParent( Region *parent, bool compute ) {

	if( parent == nullptr ) parent = &g_ui->GetScreen();

	if( m_parent == parent ) {
		return; // already set
	}
	 
	// unregister from old parent
	m_parent->RemoveChild( *this ); 
	m_parent = parent;
	m_parent->AddChild( *this );

	if( m_anchor_to_parent ) {
		ClearAnchor( false );
		//SetAnchor( parent, false );
	}
	 
	if( compute ) {
		Compute();
	}
}

//-----------------------------------------------------------------------------
void Region::AddChild( Region &child ) {

#ifdef _DEBUG

	for( auto &i : m_children ) {
		if( i == &child ) {
			assert( !"AddChild found duplicate" );
		}
	}

#endif

	m_children.push_back( &child );
}

//-----------------------------------------------------------------------------
void Region::RemoveChild( Region &child ) {
	
	for( auto i = m_children.begin(); i != m_children.end(); i++ ) {
		if( *i == &child ) {
			m_children.erase(i);
			return;
		}
	}

	assert( !"RemoveChild not found." );
}

//-----------------------------------------------------------------------------
void Region::SetAnchor( Region *anchor, bool compute ) {
	if( anchor == nullptr ) anchor = &g_ui->GetScreen();

	m_anchor_to_parent = false;
	if( m_anchor_to == anchor ) return;

	if( m_anchor_to ) {
		m_anchor_to->RemoveAnchored( *this );
	}

	m_anchor_to = anchor;
	m_anchor_to->AddAnchored( *this );

	if( compute ) Compute();
}

//-----------------------------------------------------------------------------
void Region::ClearAnchor( bool compute ) {

	m_anchor_to_parent = true;
	if( m_anchor_to == m_parent ) return;

	m_anchor_to->RemoveAnchored( *this );
	m_anchor_to = m_parent;
	m_anchor_to->AddAnchored( *this );
	
	if( compute ) Compute();
}

//-----------------------------------------------------------------------------
void Region::AddAnchored( Region &region ) {
	
#ifdef _DEBUG

	for( auto &i : m_anchor_list ) {
		if( i == &region ) {
			assert( !"AddAnchored found duplicate" );
		}
	}

#endif

	m_children.push_back( &region );
}

//-----------------------------------------------------------------------------
void Region::RemoveAnchored( Region &region ) {
	
	for( auto i = m_anchor_list.begin(); i != m_anchor_list.end(); i++ ) {
		if( *i == &region ) {
			m_anchor_list.erase(i);
			return;
		}
	}

	assert( !"RemoveAnchored not found." );
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
		   + (int)floor(point.percent * anchor.m_computed_size[set] + 0.5);
}

//-----------------------------------------------------------------------------
int Region::ComputeSize( Region &anchor, int set ) {
	
	return m_size[set].offset 
		   + (int)floor(m_size[set].percent*anchor.m_computed_size[set] + 0.5);
}

//-----------------------------------------------------------------------------
void Region::ComputeDimension( Region &anchor, int set ) {

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
void Region::Compute() {

	if( g_ui->m_computing_region == this ) {

		Console::PrintErr( 
			"Circular anchor for \"%s\".", 
			g_ui->m_computing_region->GetName() );

		return;
	}

	if( g_ui->m_computing_region == nullptr ) {
		g_ui->m_computing_region = this;
	}

	assert( GetName() != "Screen" ); // screen must never be computed.

	m_computed_valid = false;

	Region *anchor;
	
	if( m_anchor_to ) {
		anchor = m_anchor_to;
	} else {
		anchor = m_parent; 
	}

	ComputeWith( *anchor );

	// compute strata 
	
	if( m_strata == Strata::INHERIT ) {
		m_computed_strata = m_parent->m_computed_strata 
			                + STRATA_LEVEL_INCREMENT
			                + m_strata_offset;
	} else {
		m_computed_strata = StrataBase( m_strata ) + m_strata_offset;
	}
	
	// recompute linked regions
	for( auto r : m_anchor_list ) {
		r->Compute();
	}

	// finished.
	g_ui->m_computing_region = nullptr;
}

//-----------------------------------------------------------------------------
void Region::Draw() {}

//-----------------------------------------------------------------------------
}
