//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2015, Mukunda Johnson, All rights reserved. =========//

#include "stdafx.h"
#include "ui.h"

//-----------------------------------------------------------------------------
namespace Ui {

extern Instance *g_ui;

//-----------------------------------------------------------------------------
void Instance::RenderText( const Graphics::FontMaterial &font, int sort, 
						   int height, int stroke, int x, int y,
						   const Stref &text, float scale ) {

	const auto *font_charset = font.GetCharacterSet( height, stroke );
	if( !font_charset ) return;

	m_gfx_builder.New( font.m_material, sort );

	y += font_charset->GetHeight() / 4;

	float penx, peny, scalex, scaley;
	scalex = 1.0f / (float)Video::ScreenWidth();
	scaley = 1.0f / (float)Video::ScreenHeight();
	penx = (float)x * scalex;
	peny = (float)y * scaley;

	const char *ctext = *text;

	while( *ctext ) {
		char c = *ctext;

		const auto *cter = font_charset->GetCharacter(c);

		float u,v,u2,v2;
		u =      (float)cter->x / 512.0f;
		v =      (float)cter->y / 512.0f;
		u2 = u + (float)cter->w / 512.0f;
		v2 = v + (float)cter->h / 512.0f;

		float x1, y1, x2, y2;
		x1 = penx + (float)cter->left * scalex * scale;
		y1 = peny - (float)cter->top  * scaley * scale;
		x2 = x1   + (float)cter->w    * scalex * scale;
		y2 = y1   + (float)cter->h    * scaley * scale;
		
		m_gfx_builder.AddVertex( x1, y1, u,  v  );
		m_gfx_builder.AddVertex( x1, y2, u,  v2 );
		m_gfx_builder.AddVertex( x2, y2, u2, v2 );
		m_gfx_builder.AddVertex( x2, y2, u2, v2 );
		m_gfx_builder.AddVertex( x2, y1, u2, v  );
		m_gfx_builder.AddVertex( x1, y1, u,  v  );
		
		penx += ((float)cter->advance / 64.0f) * scalex * scale;

		ctext++;
	}
}

//-----------------------------------------------------------------------------
void Instance::RenderBox( const Graphics::MaterialPtr &mat, int sort,
						  int x, int y, int width, int height ) {

}

//-----------------------------------------------------------------------------
namespace Render {

//-----------------------------------------------------------------------------
void Text( Graphics::FontMaterial &f, int s, int h, int x, int y, const Stref &t ) 
                                 { g_ui->RenderText( f, s, h, 0, x, y, t, 1.0 ); }

void Box( const Graphics::MaterialPtr &mat, int sort,
						  int x, int y, int width, int height ) {

}

//-----------------------------------------------------------------------------
}}
