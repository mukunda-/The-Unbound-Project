//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2015, Mukunda Johnson, All rights reserved. =========//

#include "stdafx.h"
#include "coloredregion.h"
#include "graphics/builder.h"
#include "graphics/material.h"

//-----------------------------------------------------------------------------
namespace Ui {

Graphics::MaterialPtr g_mat;

//-----------------------------------------------------------------------------
ColoredRegion::ColoredRegion( const Stref &name ) : Region( name ) {

	g_mat = Graphics::CreateMaterial( "ColoredRegionMaterial", "ui", 
		[]( Graphics::MaterialPtr mat, const Stref & ) {
			
		});

	m_r = 255;
	m_g = 255;
	m_b = 255;
	m_a = 255;
}

//-----------------------------------------------------------------------------
void ColoredRegion::SetColor( int r, int g, int b, int a ) {
	m_r = r;
	m_g = g;
	m_b = b;
	m_a = a;
}

//-----------------------------------------------------------------------------
void ColoredRegion::Draw( Graphics::Builder &gfx ) {
	gfx.New( g_mat );

	auto rt = GetRect();

	float l,t,r,b;
	l = (float)rt[0]; t = (float)rt[1];
	r = (float)rt[2]; b = (float)rt[3];

	// debug
	l = 0;
	t = 0;
	r = 0.25;
	b = 0.25;
	 
	gfx.AddVertex( l, t, 0.0, 0.0, m_r, m_g, m_b, m_a );
	gfx.AddVertex( l, b, 0.0, 0.0, m_r, m_g, m_b, m_a );
	gfx.AddVertex( r, b, 0.0, 0.0, m_r, m_g, m_b, m_a );

	gfx.AddVertex( r, b, 0.0, 0.0, m_r, m_g, m_b, m_a );
	gfx.AddVertex( r, t, 0.0, 0.0, m_r, m_g, m_b, m_a );
	gfx.AddVertex( l, t, 0.0, 0.0, m_r, m_g, m_b, m_a );
}

}
