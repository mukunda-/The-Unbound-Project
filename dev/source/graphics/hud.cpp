//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//

#pragma warning (disable : 4244)

#include "stdafx.h"

#if 0

namespace hud {



typedef struct t_hud_drawing {
	int texture;
	Video::BlendMode blendmode;
	int start;
	int size;
	bool drawn;
} hud_drawing;

#define TEX256(u1,v1,u2,v2) ((float)(u1)/256.0f), ((float)(v1)/256.0f), ((float)(u2)/256.0f), ((float)(v2)/256.0f)

#define MAXVERTS 16384

std::vector<hud_drawing> drawings;
Video::generic_vertex2d vertex_buffer[MAXVERTS];
int vb_write_position;
hud_drawing *current_drawing;

//GLuint vboID[1];

Video::VertexBuffer vbuffer;

RasterFonts::Font *font;
const RasterFonts::CharacterSet *font_charset=0;

u8 vert_r, vert_g, vert_b, vert_a;


enum {
	TEXTURE_HUD,
	TEXTURE_TEXT,
	TEXTURE_ARCHUBOS_LOGO
};

int mode;

#include "graphics/gui_elements.h"

void init_gui();

void reset_drawings() {
	drawings.clear();
	vb_write_position = 0;
	current_drawing =0 ;
}

void new_drawing( int texture, Video::BlendMode blending ) {
	
	
	hud_drawing hd;
	hd.size = 0;
	hd.start = vb_write_position;
	hd.drawn=false;
	hd.texture = texture;
	hd.blendmode = blending;

	if( current_drawing ) {
		if( current_drawing->size == 0 )
			drawings.pop_back();
	}

	drawings.push_back(hd);
	current_drawing = &drawings[drawings.size()-1];
}

void init() {
	
	//glGenBuffersARB( 1, vboID );
	vbuffer.Create( Video::VF_GENERIC2D );
	vb_write_position = 0;
	reset_drawings();

	init_gui();
}

void use_font( RasterFonts::Font *source ) {
	font = source;
}

void set_vertex_color( u8 r, u8 g, u8 b, u8 a ) {
	vert_r = r;
	vert_g = g;
	vert_b = b;
	vert_a = a;
}

void set_vertex_color( const hud_color &c ) {
	vert_r = c.r;
	vert_g = c.g;
	vert_b = c.b;
	vert_a = c.a;
}

void set_text_font( int height, int stroke ) {
	const RasterFonts::CharacterSet *cs = font->GetCharacterSet( height, stroke );
	font_charset = cs;
}
void set_text_font( gui_font &f ) {
	int height, stroke;
	if( f.get_params( height, stroke ) ) {
		set_text_font( height, stroke );
	} else {
		set_text_font( 24, 0 ); // todo, inherit fonts
	}
}

void set_vertex_texture( u8 t, Video::BlendMode blendmode ) {
	if( current_drawing ) {
		if( t == current_drawing->texture && blendmode == current_drawing->blendmode ) return;
	}
	new_drawing( t, blendmode );
}

void set_vertex_texture( u8 t ) {
	set_vertex_texture( t, Video::BLEND_ALPHA );
}

/*
void add_vertex_text( float x, float y, float u, float v ) {
	if( buffer_text_write >= 8192 ) return;
	hud_vertex *w = buffer_text+buffer_text_write;
	buffer_text_write++;
	w->x = x;
	w->y = y;
	w->u = u;
	w->v = v;
	w->r = vert_r;
	w->g = vert_g;
	w->b = vert_b;
	w->a = vert_a;
}*/

void add_vertex( float x, float y, float u, float v ) {
	if( vb_write_position >= MAXVERTS ) return;
	current_drawing->size++;
	Video::generic_vertex2d *w = vertex_buffer+vb_write_position;
	vb_write_position++;
	w->x = x;
	w->y = y;
	w->u = u;
	w->v = v;
	w->r = vert_r;
	w->g = vert_g;
	w->b = vert_b;
	w->a = vert_a;
}

void draw_text_clipped( int x, int y, int scroll, int width, const char *text ) {

	if( !font_charset ) return;
	
	y += font_charset->get_height()/4;

	float penx, peny, scalex, scaley;
	scalex = 1.0f / (float)Video::ScreenWidth();
	scaley = 1.0f / (float)Video::ScreenHeight();
	penx = (float)x ;
	peny = (float)y ;

	penx -= scroll ;

	set_vertex_texture( Textures::FONT1 ); 
	
	while( *text ) {
		char c = *text;

		float u,v,u2,v2;
		u = (float)font_charset->GetCharacter(c)->x / 512.0f;
		v = (float)font_charset->GetCharacter(c)->y / 512.0f;
		u2 = u + (float)font_charset->GetCharacter(c)->w / 512.0f;
		v2 = v + (float)font_charset->GetCharacter(c)->h / 512.0f;

		float x1, y1, x2, y2;
		x1 = penx + (float)font_charset->GetCharacter(c)->left ;
		y1 = peny - (float)font_charset->GetCharacter(c)->top   ;
		x2 = x1 + (float)font_charset->GetCharacter(c)->w   ;
		y2 = y1 + (float)font_charset->GetCharacter(c)->h   ;
		
		if( !(penx + (x2-x1) < x || penx >= (x+width) ) ) {
		
			if( penx < x ) {
				x1 += (x-penx) ;
				u += (x-penx) /512.0f;


			} else if( penx > (x+width-(x2-x1)) ) {
				u2 -= (penx+(x2-x1)-(x+width)) / 512.0f;
				x2 -= penx+(x2-x1)-(x+width);
			}

			x1 *= scalex;
			y1 *= scaley;
			x2 *= scalex;
			y2 *= scaley;
			add_vertex( x1, y1, u,  v );
			add_vertex( x1, y2, u,  v2 );
			add_vertex( x2, y2, u2, v2 );
			add_vertex( x2, y1, u2, v );
		}
		
		penx += ((float)font_charset->GetCharacter(c)->advance / 64.0f)  ;

		text++;
	}
}

void draw_text_scaled( int x, int y, const char *text, float scale ) {

	if( !font_charset ) return;

	scale=1.0;
	y += font_charset->get_height()/4;

	float penx, peny, scalex, scaley;
	scalex = 1.0f / (float)Video::ScreenWidth();
	scaley = 1.0f / (float)Video::ScreenHeight();
	penx = (float)x * scalex;
	peny = (float)y * scaley;



	set_vertex_texture( Textures::FONT1 );
	
	while( *text ) {
		char c = *text;

		float u,v,u2,v2;
		u = (float)font_charset->GetCharacter(c)->x / 512.0f;
		v = (float)font_charset->GetCharacter(c)->y / 512.0f;
		u2 = u + (float)font_charset->GetCharacter(c)->w / 512.0f;
		v2 = v + (float)font_charset->GetCharacter(c)->h / 512.0f;

		float x1, y1, x2, y2;
		x1 = penx + (float)font_charset->GetCharacter(c)->left * scalex * scale;
		y1 = peny - (float)font_charset->GetCharacter(c)->top * scaley * scale;
		x2 = x1 + (float)font_charset->GetCharacter(c)->w * scalex * scale;
		y2 = y1 + (float)font_charset->GetCharacter(c)->h * scaley * scale;
		
		
		add_vertex( x1, y1, u,  v );
		add_vertex( x1, y2, u,  v2 );
		add_vertex( x2, y2, u2, v2 );
		add_vertex( x2, y1, u2, v );
		
		penx += ((float)font_charset->GetCharacter(c)->advance / 64.0f) * scalex * scale;

		text++;
	}

}

void draw_text( int x, int y, const char *text ) {
	draw_text_scaled( x, y, text, 1.0f );
}

void draw_text_centered_scaled( int x, int y, const char *text, float scale ) {

	if( !font_charset ) return;
	const char *t = text;

	float penx=0;//, peny, scalex, scaley;
	//scalex = 1.0f / (float)Video::SCREEN_WIDTH;
	//penx = (float)x * scalex;

	while( *t ) {
		char c = *t;
		penx += ((float)font_charset->GetCharacter(c)->advance / 64.0f) * scale;
		t++;
	}
	
	draw_text_scaled( (int)(x - penx/2.0f), y, text, scale );
}

void draw_text_centered( int x, int y, const char *text ) {
	draw_text_centered_scaled( x, y, text, 1.0f );
}


void add_rect( float x, float y, float w, float h, float u1, float v1, float u2, float v2 ) {
	//if( buffer_write >= 8192 ) return;

	//x = x / (float)Video::SCREEN_WIDTH;
	//y = y / (float)Video::SCREEN_HEIGHT;
	float x1,x2,y1,y2;
	x1 = x/(float)Video::ScreenWidth();
	y1 = y/(float)Video::ScreenHeight();

	x2 = (x+w)/(float)Video::ScreenWidth();
	y2 = (y+h)/(float)Video::ScreenHeight();

	add_vertex( x1, y1, u1, v1 );
	add_vertex( x1, y2, u1, v2  );
	//add_vertex( x2, y1, u2, v1  );
	add_vertex( x2, y2, u2, v2  );
	add_vertex( x2, y1, u2, v1  );
	//add_vertex( x1, y2, u1, v2  );
	
}

void add_rect_centered( float x, float y, float w, float h, float u1, float v1, float u2, float v2 ) {
	add_rect( x - w/2, y - h/2, w, h, u1, v1, u2, v2 );
}

void add_rect2( float x, float y, float w, float h, float u1, float v1, float uw, float vw ) {
	add_rect( x, y, w, h, u1, v1, u1+uw, v1+vw );
}

//void add( float x, float y, float w, float h, float sx, float sy ) {
//	add_rect( x, y, w, h, sx, sy, w, h  );
//}
/*
void draw_bar_contents( int x, int y, int width, int type ) {
	for( int i = 0; i < width; i += 16 ) {
		int u, v=80, w=16;
		if( i == 0 ) {
			u = 64;
			
		} else if( i == width-16 ) {
			u = 112;
		} else if( i & 16 ) {
			u = 80;
		} else { 
			u = 96;
		}
		if( i + w > width ) w = width-i;
		u += type * 64;
		add_rect2( (float)(x+i), (float)y, w, 32, u, v, (float)w/256.0f,  );
	}
}*/
/*
void draw_bar( int x1, int y, int width, float filled, int type ) {
	
	//add_ex( (float)x1+4, (float)y,   ((float)width-8)*filled, 32, 74, 80, 16,32, 1.0 );
	for( int i = 16; i < width-16; i += 16 ) {
		add( (float)(x1+i),(float)y,16,32,32 - ((i&16)?16:0),80 );
	}
	add( (float)x1, (float)y,16,32,0,80 );
	add( (float)(x1+width-16),(float)y,16,32,48,80 );

	//for( int i = 16; i < width-16; i += 16 ) {
	//	add( (float)(x1+i),(float)y,16,32,32 - ((i&16)?16:0),80,1.0);
	//}
	draw_bar_contents( x1, y, width*filled, type );
}*/

void draw_slot( float x, float y, float width, float height ) {
	
	add_rect2( x, y, width, height, 0, 0, 64.0f/256.0f,64.0f/256.0f );
}

void set_mode_real( int newmode ) {
	mode = newmode;

	GUI.clear();

	switch( mode ) {
	case MODE_LIVE:
		Input::LockMouse();
		break;
	case MODE_TITLE:
		Input::UnlockMouse();
		//GUI.add_page(&gp_title);
		GUI.add_page( &gp_test );
		break;
	default:
		Input::UnlockMouse();
	}
}

void set_mode( int newmode ) {
	if( newmode != mode ) {
		set_mode_real(newmode);
	}
}

int get_mode() {
	return mode;
}

void update() {
	
}

void draw_slot_array( float x, float y, float size, float padding, int xcount, int ycount, int *pictures ) {

	for( int ix = 0; ix < xcount; ix++ ) {
		for( int iy = 0; iy < ycount; iy++ ) {
			draw_slot( x + (float)ix * (size + padding) + padding/2, y + (float)iy * (size + padding)+ padding/2, size, size );
		}
	}
}

void draw_highlight( float x, float y, float width, float height  ) {
	int padding = 4;
	
	add_rect2( x + padding, y + padding, width-padding*2, height- padding*2, TEX256(192,64,64,64) );
}

void draw_primitive( float x, float y, float width, float height, bool out ) {
	int padding = 4;
	if( !out ) 
		add_rect2( x + padding, y + padding, width-padding*2, height- padding*2, 136.0f/256.0f, 8.0f/256.0f, 54.0f/256.0f,54.0f/256.0f  );
	else
		add_rect2( x + padding, y + padding, width-padding*2, height- padding*2, 192.0f/256.0f, 8.0f/256.0f, 54.0f/256.0f,54.0f/256.0f  );

	//corners
	add_rect2( x, y, 8, 8, TEX256(64,0,8,8) );
	add_rect2( x + width - 8, y, 8, 8, TEX256(80, 0,8,8)  );
	add_rect2( x, y + height - 8, 8, 8, TEX256(104, 0,8,8)  );
	add_rect2( x + width - 8, y + height - 8, 8, 8, TEX256(120, 0, 8, 8) );
	if( width > 16 ) {
		// top/bottom
		add_rect2( x+8, y, width-16, 8, TEX256(72, 0, 8, 8) );
		add_rect2( x+8, y+height-8, width-16, 8, TEX256(112, 0, 8, 8) );
	}

	if( height > 16 ) {
		// left/right
		add_rect2( x, y+8, 8, height-16, TEX256(88,1,8,46) );
		add_rect2( x+width-8, y+8, 8, height-16, TEX256(96,1,8,46) );
	}
}

void draw_button( float x, float y, float width, float height, bool out ) {
	draw_primitive( x , y  , width,height,out );
}

void draw_slot_array_centered( float x, float y, float size, float padding, int xcount, int ycount, int *pictures ) {
	draw_slot_array( x - (size+padding)*xcount/2, y - (size+padding)*ycount/2, size, padding, xcount, ycount, pictures );
}

void draw_bar2( float x, float y, float width, float height, float filled, u8 r, u8 g, u8 b, u8 a ) {

	set_vertex_color(255,255,255,a);
	draw_primitive( x,y,width,height,true );

	set_vertex_color(r/4,g/4,b/4,a/2);//64);
	add_rect2( x + 4, y+4, (width-8)  , (height-8), TEX256(0, 224, width , 32) );

	set_vertex_color(r,g,b,a);
	add_rect2( x + 4, y+4, (width-8) * filled, (height-8), TEX256(0, 224, width*filled, 32) );
	
}

void draw_bar2_centered( float x, float y, float width, float height, float filled, u8 r, u8 g, u8 b, u8 a ) {
	draw_bar2( x - width/2, y - height/2, width, height, filled, r, g, b, a );
}
/*
void draw_pushbutton( float x, float y, float width, float height, 
						u8 r, u8 g, u8 b, u8 a, 
						u8 tr, u8 tg, u8 tb, u8 ta, 
						bool pressed, const char *caption ) {
	
	
	
}*/


void draw() {
	// crosshair
	set_vertex_texture( Textures::HUD );
	set_vertex_color(255,255,255,255);
	add_rect2( (float)Video::ScreenWidth()/2.0f-7.0f, (float)Video::ScreenHeight()/2.0f-7.0f , 16.0f, 16.0f, TEX256(16, 128,16,16) );

	//draw_slot_array( 40, 40, 48.0, 2, 15, 9, NULL );

	//draw_slot_array_centered( Video::SCREEN_WIDTH/2, Video::SCREEN_HEIGHT / 2, 48, 2, 12, 6, NULL );
	
	set_vertex_color(255,255,255,255);
	//draw_slot_array_centered( Video::SCREEN_WIDTH/2, Video::SCREEN_HEIGHT - 40, 64, 8, 6, 1, NULL );

	//draw_bar2_centered( Video::SCREEN_WIDTH/2, Video::SCREEN_HEIGHT-40-44-20, 6*(64+8)-8, 18, 0.4f, 237, 28, 36, 255 );
	//draw_bar2_centered( Video::SCREEN_WIDTH/2, Video::SCREEN_HEIGHT-40-44, 6*(64+8)-8, 18, 0.4f, 0, 84, 166, 255 );

	set_vertex_color( 0, 0, 0, 64 );
	//add_rect2( 0, Video::SCREEN_HEIGHT-4, Video::SCREEN_WIDTH, 4, TEX256(0,224,256,32) );


 	//set_vertex_texture( Textures::THINGY);
 	//set_vertex_color( 255, 255, 255, 255 );
 	//add_rect(0,0,1920,1080,0.4,0.4,1920/128.0 ,1080/128.0 );

	/////////////////////////////////////////////////
	//test_bar[0].set_filled( gametime::getSecondsMod( 1.0 ) );
	//test_bar[1].set_filled( gametime::getSecondsMod( 2.0 )/2.0 );
	///test_bar[2].set_filled( gametime::getSecondsMod( 3.0 )/3.0 );
	
//	testbar.set_filled( testbar.get_filled() + 0.01 );
	GUI.draw();
	
	 
	set_text_font( 24,0);
	set_vertex_color(255,255,255,255); 
	

	char poop[200] = "ARCHUBOS WC " ;
	
	_strdate(poop+12);
	hud::draw_text( 5, 30, poop);//__DATE__ "/" __TIME__  );
	
}

//-------------------------------------------------------------------------------------------------------------------------
void render() {
//-------------------------------------------------------------------------------------------------------------------------

	
	vbuffer.BufferData( vertex_buffer, vb_write_position * sizeof(Video::generic_vertex2d), GL_STREAM_DRAW_ARB );
	
	// render data
	//Shaders::HS.Use();
	
	//glEnableVertexAttribArray( Shaders::HS.aPosition );
	//glEnableVertexAttribArray( Shaders::HS.aTexCoord );
	//glEnableVertexAttribArray( Shaders::HS.aColor );
	Video::SetDepthBufferMode( Video::ZBUFFER_DISABLED );

	// copy data to buffer
	//glBindBufferARB( GL_ARRAY_BUFFER_ARB, vboID[0] );
	//glBufferDataARB( GL_ARRAY_BUFFER_ARB, vb_write_position * sizeof(hud_vertex), vertex_buffer, GL_STATIC_DRAW_ARB );
		
	//glUniform3f( Shaders::HS.uTranslate, 0.0f, 0.0f, 0.0f );
	//glUniform1i( Shaders::HS.uSampler, 0 );
	//glBindBufferARB(GL_ARRAY_BUFFER_ARB, *vboID );
	
	Video::Shaders::HUDShader *shader = Video::Shaders::Find( "hud" );

	shader->Use();
	shader->SetVertexAttributePointers( 0 );
	//	Video::SetShader( Video::SHADER_HUD );
	//Video::SetShaderVertexAttributePointers( 0 );

	
	for( u32 i = 0; i < drawings.size(); i++ )  {
		
		hud_drawing *hd = &drawings[i];
		
		Textures::Bind( hd->texture );

		Video::SetBlendMode( hd->blendmode );
	//	glBlendFunc(GL_SRC_ALPHA, hd->additive_blending ? GL_ONE : GL_ONE_MINUS_SRC_ALPHA );

 	//	glVertexAttribPointer( Shaders::HS.aTexCoord, 2, GL_FLOAT, GL_FALSE, 20, (void*)0 );
	//	glVertexAttribPointer( Shaders::HS.aColor, 4, GL_UNSIGNED_BYTE, GL_TRUE, 20, (void*)8 );
	//	glVertexAttribPointer( Shaders::HS.aPosition, 2, GL_FLOAT, GL_FALSE, 20, (void*)12 );

		Video::DrawQuads( hd->start, hd->size );
	//	glDrawArrays(GL_QUADS, hd->start, hd->size );
	}

	//glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	
	//glDisableVertexAttribArray( Shaders::HS.aPosition );
	//glDisableVertexAttribArray( Shaders::HS.aTexCoord );
	//glDisableVertexAttribArray( Shaders::HS.aColor );
		
	// release vbo
	//glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);

	Video::SetDepthBufferMode( Video::ZBUFFER_ENABLED );
	//glEnable( GL_DEPTH_TEST );

	reset_drawings();
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int gui_padding() {
	return 4;
}

hud_color hot_overlay_color() {
	return hud_color(0,174,250,64);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void get_item_icon_uv( int index, float &u1, float &v1, float &u2, float &v2 ) {
	index--;

	u1 = (float)(index & 7) * 1.0f/8.0f;
	v1 = (float)(index >> 3) * 1.0f/8.0f;
	u2 = u1 + (31.0f/256.0f);
	v2 = v1 + (31.0f/256.0f);

}

void gui_slot::draw() {
	float dx, dy;
	translate( dx, dy );

	
	set_vertex_color(255,255,255,255);
	set_vertex_texture( Textures::HUD );
	draw_primitive( dx, dy, width, height, false );

	if( hot ) {
		set_vertex_color(hot_overlay_color());
		draw_primitive( dx, dy, width, height, false );
	}

	

	if( item ) {
		set_vertex_color(255,255,255, dragging ? 128 : 255);
		set_vertex_texture( Textures::ITEMS );

		float u1, v1, u2, v2;
		get_item_icon_uv( item, u1, v1, u2, v2 );
		add_rect( dx+4, dy+4, width-8, height-8, u1, v1, u2, v2 );
	}

	if(pressed){
		set_vertex_color(88,199,255,255);
		set_vertex_texture( Textures::HUD, Video::BLEND_ADD );
		draw_highlight(dx,dy,width,height);
		//draw_primitive( dx, dy, width, height, false );
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void gui_pushbutton::set_caption( const char *pcaption ) {
	caption = pcaption;
}

void gui_pushbutton::set_textcolor( const hud_color &col ) {
	text_color = col;
}

void gui_pushbutton::draw() {
	float dx, dy ;
	translate( dx, dy );

	set_vertex_color(255,255,255,255);

	set_vertex_texture( Textures::HUD );
	draw_button( dx, dy, width, height, !pressed );
	if( hot ) {
		set_vertex_color(hot_overlay_color());
		draw_button( dx, dy, width, height, !pressed );
	}

	set_text_font(font);
	
	float cx = dx + width/2;
	float cy = dy + height/2;
	set_vertex_color(0,0,0,text_color.a/4);
	draw_text_centered( (int)cx+1, (int)cy+1 + (pressed ? 2 : 0), caption.c_str() );
	set_vertex_color(text_color);
	draw_text_centered( (int)cx-1, (int)cy-1 + (pressed ? 2 : 0), caption.c_str() );


	//draw_pushbutton( dx, dy, width, height, 255,255,255,255,
	//	text_color.r, text_color.g, text_color.b, text_color.a,
	//	pressed, caption.c_str() );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

float translate_caret_position( std::string &text, int pos ) {

	if( !font_charset ) return 0;
	float x = 0.0;
	for( int i = 0; i < pos; i++ )
		x += ((float)font_charset->GetCharacter(text[i])->advance / 64.0f) ; // TODO: POTENTIALLY INSECURE (?)
	return x;
}

float measure_text( std::string &text ) {


	if( !font_charset ) return 0;
	u32 size = text.size();
	float width=0;
	u32 i;
	for( i = 0; i < (size-1); i++ ) {
		width += font_charset->GetCharacter(text[i])->advance / 64.0f;
	}
	
	if( i != 0 ) {
		width += font_charset->GetCharacter(text[i])->w;
	}
	return width;
}

void gui_textbox::reset_caret_blink() {
	caret_blink = gametime::getSecondsMod( 1.0 );
}

void gui_textbox::set_caret_position_pixels( int px ) {
	
	set_text_font( font );

	px -= 8;
	float x = 0.0;
	caret = 0;

	if( font_charset ) {
		for( u32 i = 0; i < text.size(); i++ ) {
			x += ((float)font_charset->GetCharacter(text[i])->advance / 64.0f) ; // TODO: POTENTIALLY INSECURE (?)
			if( x > (px+scroll) ) break;
			caret++;
		}
	}
	
}

void gui_textbox::scroll_to_caret() { 
	// find caret position
	
	set_text_font( font );

	float pos = translate_caret_position( text, caret );

	if( font_charset ) {
		if( caret != 0 )
			pos += font_charset->GetCharacter(text[caret-1])->w - font_charset->GetCharacter(text[caret-1])->advance/64;
	}
	float padding = 16.0;

	if( pos < scroll )
		scroll = pos;
	if( pos >= scroll+width-padding ) {
		scroll = pos-(width-padding)-1;
	}
	if( scroll < 0 ) scroll = 0;
}
 

void gui_textbox::windows_message( UINT msg, WPARAM wParam, LPARAM lParam) {
	

	set_text_font( font );
	// todo: paste
	switch( msg ) {
	case WM_CHAR:
		if( (wParam >= 32 && wParam < 128) || wParam == 8 ) {
			char c = (char)wParam;
			int repeat = lParam & 65535;
			for( ;repeat--; ) {
				if( c == 8 ) {
					if( caret != 0 ) {
						text.erase( caret-1, 1 );
						caret--;
					}
				} else {
					text.insert( caret, 1, c );
					caret++;
				}
			}
			reset_caret_blink();
			scroll_to_caret();
		}
		break;
	case WM_KEYDOWN:
		{
			int repeat = lParam & 65535;
			
			switch( wParam ) {
			case VK_LEFT:
				caret -= repeat;
				if( caret < 0 ) caret = 0;
				scroll_to_caret();
				reset_caret_blink();
				break;
			case VK_RIGHT:
				caret += repeat;
				if( caret > (int)text.size() ) caret = text.size();
				scroll_to_caret();
				reset_caret_blink();
				break;
			case VK_HOME:
				caret = 0;
				scroll_to_caret();
				reset_caret_blink();
				break;
			case VK_END:
				caret = text.size();
				scroll_to_caret();
				reset_caret_blink();
				break;
			case VK_DELETE:
				if( caret != text.size() ) {
					text.erase(caret,1);
				}
				break;
			}
		}
		break;
	case WM_LBUTTONDOWN:
		reset_caret_blink();
		float dx,dy;
		translate(dx,dy);
		int px = Input::MouseX() - (int)dx;
		set_caret_position_pixels( px );
		break;
	}

	gui_object::windows_message( msg, wParam, lParam );
}

/* 
void gui_textbox::raise_event( int msg ) {
	switch( msg ) {
	case EVENT_KEYINPUT:
		char c;
		reset_caret_blink();
		while( c = Input::ReadCharBuffer() ) {
			
			if( c == 8 ) {
				if( caret != 0 ) {
					text.erase( caret-1, 1 );
					caret--;
				}
			} else {
				text.insert( caret, 1, c );
				caret++;
			}
		}
		scroll_to_caret();
		break;

	case EVENT_CLICKED:
		reset_caret_blink();
		float dx,dy;
		translate(dx,dy);
		int px = Input::MouseX() - dx;
		set_caret_position_pixels( px );
	}

	gui_object::raise_event(msg);
}*/

void gui_textbox::draw() {

	float dx, dy ;
	translate( dx, dy );

	set_vertex_texture( Textures::HUD );
	set_vertex_color( 255,255,255,255 );
	draw_primitive( dx, dy, width, height, false );
	if( hot ) {
		set_vertex_color( hot_overlay_color() );
		draw_primitive( dx, dy, width, height, false );
	}

	
	set_text_font( font );

	if( !font_charset ) return;

	
	


	set_vertex_color( 0,0,0,64 );
	draw_text_clipped( (int)(dx+8+1), (int)(dy + height/2+1), (int)scroll, (int)(width-16.0f), text.c_str() );
	set_vertex_color( 255,255,255,255 );
	draw_text_clipped( (int)(dx+8-1), (int)(dy + height/2-1), (int)scroll, (int)(width-16.0f), text.c_str() );

	set_vertex_color( 255,255,255,255 );
	set_vertex_texture( Textures::HUD );
	// draw caret
	if( focused ) {
		if( gametime::getSecondsMod( 1.0, 1.0-caret_blink ) < 0.5 ) {
			float cx = translate_caret_position(text,caret) - scroll;
			add_rect2( dx + 8 - 2 + cx, dy + height/2+1 - font_charset->get_height()*0.75, 4, font_charset->get_height(), TEX256(0,146,4,4) );
		}
	}
	
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void gui_label::set_color( const hud_color &ptext_color ) {
	text_color = ptext_color;
}

void gui_label::set_caption( const char *pcaption ) {
	
	set_text_font( font );

	caption = pcaption;
	width = measure_text( caption );
}

void gui_label::draw() {
	
	set_text_font( font );

	float dx, dy;
	translate( dx, dy );

	set_vertex_color( 0,0,0,text_color.a/2);
	draw_text( dx+1, dy + height/2+1, caption.c_str() );
	set_vertex_color(text_color);
	draw_text( dx-1, dy + height/2-1, caption.c_str() );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void gui_bar::set_filled( float pfilled ) {
	filled = pfilled;
	if( filled < 0 ) filled =0 ;
	if( filled > 1.0f ) filled = 1.0f;
}

float gui_bar::get_filled() {
	return filled;
}

void gui_bar::draw() {
	float dx, dy;
	translate(dx,dy);

	set_vertex_texture( Textures::HUD );
	draw_bar2( dx, dy, width, height, filled, color.r, color.g, color.b, color.a );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void gui_image::set_image( GLuint ptexture, const hud_color &pcol, float pu1, float pv1, float pu2, float pv2 ) {
	texture = ptexture;
	color = pcol;
	u1 = pu1;
	u2 = pu2;
	v1 = pv1;
	v2 = pv2;
}

void gui_image::draw() {
	set_vertex_color( color );
	set_vertex_texture( texture );

	float dx, dy ;
	translate( dx, dy );

	add_rect( dx, dy, width, height, u1, v1, u2, v2 );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void gui_scroller::set_value( float v ) {
	value = v;
}

float gui_scroller::get_value() {
	return value;
}

void gui_scroller::set_vertical( bool v ) {
	vertical = v;
}

void gui_scroller::get_thumb_min_max( float &min, float &max ) {
	float thumbsize = get_thumbsize();
	min = 4 + thumbsize/2;
	if( vertical ) {
		
		max = height - 4 - thumbsize/2;
	} else {
		max = width - 4 - thumbsize/2;
	}
}

void gui_scroller::mouse_position( float mx, float my ) {
	float min, max;
	gui_scroller::get_thumb_min_max( min, max );
	if( vertical ) {
		value = (my - min) / (max-min);
	} else {
		value = (mx - min) / (max-min);
	}
	if( value < 0.0f ) value = 0.0f;
	if( value > 1.0f ) value = 1.0f;
	
}

void gui_scroller::windows_message( UINT msg, WPARAM wParam, LPARAM lParam) {
	switch( msg ) {
	case WM_MOUSEMOVE:
		if( !(wParam & MK_LBUTTON) ) break;
	case WM_LBUTTONDOWN:
		float dx, dy;
		translate( dx,dy );
		float mx = Input::MouseX() - dx;
		float my = Input::MouseY() - dy;
		gui_scroller::mouse_position( mx, my );
		break;
	}
}

float gui_scroller::get_thumbsize() {
	float thumbsize;
	thumbsize = width-8;
	if( thumbsize > height-8 ) thumbsize = height-8;
	return thumbsize;
}

void gui_scroller::draw() {
	float dx, dy;
	translate( dx, dy );

	set_vertex_color(255,255,255,255);
	set_vertex_texture( Textures::HUD );

	draw_primitive( dx, dy, width, height, false );

	float thumbsize = get_thumbsize();

	float min,max;
	get_thumb_min_max(min,max);

	if( vertical ) {
		
		add_rect2( dx + 4, dy + floor(min + (max-min) * value + 0.5 - thumbsize/2), thumbsize, thumbsize, TEX256( 65, 129, 30, 30 ) );
	} else {
		
		add_rect2( dx + floor(min + (max-min) * value + 0.5 - thumbsize/2), dy + 4, thumbsize, thumbsize, TEX256( 65, 129, 30, 30 ) );
	}

	if( hot ) {
		set_vertex_color(hot_overlay_color());

		draw_primitive( dx, dy, width, height, false );
	}
	if( held ){
		set_vertex_color(88,199,255,255);
		set_vertex_texture( Textures::HUD, Video::BLEND_ADD );
		draw_highlight(dx,dy,width,height);
		//draw_primitive( dx, dy, width, height, false );
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void gui_object::set_anchor( gui_object *p_parent, int p_h_align, int p_v_align ) {
	anchor = p_parent;
	h_align = p_h_align;
	v_align = p_v_align;
}

void gui_object::translate( float &dx, float &dy ) {

	float x1,y1,x2,y2; // parent rect

	if( anchor != NULL ) {
		anchor->translate_rect( x1, y1, x2, y2 );
		
	} else {
		x1 = 0;
		y1 = 0;
		x2 = Video::ScreenWidth();
		y2 = Video::ScreenHeight();
	}

	float xref;
	switch( h_align >> 2 ) {
		case 0: xref = x1; break;
		case 2: xref = x2; break;
		default: xref = (x1+x2)/2; break;
	}
	xref += x;

	switch( h_align & 3 ) {
	case 0:
		dx = xref - width - h_padding;
		break;
	case 1:
		dx = xref - width/2;
		break;
	case 2:
		dx = xref + h_padding;
		break;
	}
	
	float yref;
	switch( v_align >> 2 ) {
		case 0: yref = y1; break;
		case 2: yref = y2; break;
		default: yref = (y1+y2)/2; break;
	}
	yref += y;

	switch( v_align & 3 ) {
	case 0:
		dy = yref - height - v_padding;
		break;
	case 1:
		dy = yref - height/2;
		break;
	case 2:
		dy = yref + v_padding;
		break;
	}

}

void gui_object::translate_rect( float &dx1, float &dy1, float &dx2, float &dy2 ) {
	translate(dx1,dy1);
	dx2 = dx1 + width;
	dy2 = dy1 + height;
}

void gui_object::set_rect( float X, float Y, float WIDTH, float HEIGHT ) {//, bool CENTERED ) {
	x = X;
	y = Y;
	width = WIDTH;
	height = HEIGHT;
	//centered = CENTERED;
}

void gui_object::set_position( float X, float Y ) {
	x = X;
	y = Y;
}

void gui_object::drop( gui_object *source ) {
	windows_message( WM_USER, USER_MSG_DROP, (int)source );
}

void gui_object::nulldrag( ) {
	windows_message( WM_USER, USER_MSG_NULLDRAG, 0 );
}

bool gui_object::picked( float px, float py ) {
	float dx, dy;
	translate( dx, dy );

	return ( px >= dx && px < dx+width && py >= dy && py < dy+height );
}

/*
void gui_object::raise_event( int msg ) {
	if( handler ) {
		switch( msg ) {
		case EVENT_CLICKED:
			float dx, dy;
			translate( dx, dy );
			handler( msg, Input::MouseX() - (int)dx, Input::MouseY() - (int)dy, 0, 0);
			break;
		case EVENT_KEYINPUT:
			break;
		}
	}
}*/

void gui_object::windows_message( UINT msg, WPARAM wParam, LPARAM lParam ) {
	if( handler ) {
		handler( (void*)this, msg, wParam, lParam );
	}
}

void gui_object::draw() {
	// default function
}

void gui_object::set_user_data( void *data, int tag, bool autofree ) {
	 user_data = data; 
	 user_data_needs_to_be_deleted = autofree;
	user_data_tag = tag;
}

int gui_object::get_user_data( void **data ) {
	if( data ) {
		*data = user_data;
	}
	return user_data_tag;
}

void gui_object::set_item( int i ) { item = i; }
int gui_object::get_item() { return item; }

void gui_object::swap_item( gui_object &swap ) {
	int i = swap.get_item();
	swap.set_item(item);
	item = i;
}

void gui_object::remove_item() {
	item = 0;
}

gui_object::gui_object() {
	allocated=false;
	focused = false;
	h_align = HALIGN_CENTER;
	v_align = VALIGN_CENTER;
	h_padding = gui_padding();
	v_padding = gui_padding();
	x=0;
	y=0;
	width=0;
	height=0;
	anchor = 0;

	user_data=0;
	pressed=false;
	centered=true; // (still used?)
	hot=false;
	dragging=false;
	is_draggable=false;
	is_click_draggable=false;
	user_data=0;
	user_data_tag = 0;
	user_data_needs_to_be_deleted=false;
	handler=0;
	item=0;
	accept_dnd=false;
}

gui_object::~gui_object() {
	if( user_data ) {
		if( user_data_needs_to_be_deleted ) {
			delete user_data;
		}
	}
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

gui_page::gui_page() {}

gui_page::~gui_page() {
	for( u32 i = 0; i < objects.size(); i++ ) {
		if( objects[i]->delete_on_exit() ) {
			delete objects[i];
		}
	}
}

gui_object * gui_page::pick( float x, float y ) {
	for( u32 i = 0; i < objects.size(); i++ ) {
		if( objects[i]->picked( x, y ) ) {
			return objects[i];
		}
	}
	return NULL;
}

void gui_page::add( gui_object *a ) {
	objects.push_back(a);
}

void gui_page::draw() {
	for( u32 i = 0; i < objects.size(); i++ ) {
		if( objects[i] ) {
			objects[i]->draw();
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

gui::gui() {
	drag_mode = false;
}

void gui::clear() {
	pages.clear();
}

void gui::add_page( gui_page *p ) {
	pages.push_back(p);
}

void gui::release_focus() {
	if( focus ) {
		focus->setfocus(false);
		focus->setpressed(false);
		focus->setheld(false);
		focus =0 ;
		focusheld=false;
	}
}

void gui::set_hot( gui_object *newhot ) {
	if( hot ) hot->sethot(false);
	hot = newhot;
	if( hot ) hot->sethot(true);
}

gui_object * gui::pick_object() {
	float mx = Input::MouseX();
	float my = Input::MouseY();
	for( u32 i = 0; i < pages.size(); i++ ) {
		if( pages[i] ) {
			gui_object *o = pages[i]->pick( mx, my );

			if( o ) {
				return o;
			}
		}
	}
	return NULL;
}

void gui::set_drag_mode( gui_object *source ) {
	drag_mode = true;
	drag_source = source;
	set_cursor( source->get_item() );
	source->set_dragging(true);
}

void gui::cancel_drag() {
	drag_mode = false;
	drag_source->set_dragging(false);
	set_cursor(0);
	drag_source=  0;
}

void gui::drag_drop( gui_object *target ) {

	if( target == drag_source ) {
		cancel_drag();
		return;
	}
	drag_mode = false;
	drag_source->set_dragging(false);
	set_cursor( 0 );
	if( target ) {
		target->drop( drag_source );
	} else {
		drag_source->nulldrag();
	}

	drag_source = 0;
	
}


void gui::on_mousedown() {
	float mx = Input::MouseX();
	float my = Input::MouseY();

	
	gui_object *o = pick_object();

	if( o ) {
					
		release_focus();
		focus = o;
		o->setfocus(true);

		if( !drag_mode ) {


			focusheld=true;
			focus->setpressed(true);
			focus->setheld(true);
		} else {

			if( o->get_accept_dnd() ) {
				drag_drop( o );
			} else {
				drag_drop( 0 );
			}
					
		}

		mousedownpos = cml::vector2i( mx, my );
	} else {
		if( drag_mode ) {
			drag_drop(0);
		}
	}
	
}

bool gui::on_mousemove() {
	float mx = Input::MouseX();
	float my = Input::MouseY();
	bool rval=false;
	if( Input::MousePressed(0) ) {
		if( focus ) {
			if( focusheld ) {
				rval=true;
				if( focus->picked( mx, my ) ) {
					focus->setpressed(true);
				} else {
					focus->setpressed(false);
				}

				if( cml::length_squared( mousedownpos - cml::vector2f(mx,my) ) >9*9 ) {
					if( focus->draggable() ) {
						set_drag_mode( focus );
						focus->setpressed(false);
						focus->setheld(false);
						focusheld=false;
					}
				}

			}

			
		}
	} else {
		if( focus ) {
			if( focus->picked( mx,my) ) {
				rval=true;
			}
		}
	}
	set_hot( pick_object() );
	return rval;
}

bool gui::on_mouseup() {
	if( !drag_mode ) {
		if( focus ) {
			if( focusheld ) {
				focusheld=false;
				focus->setpressed(false);
				focus->setheld(false);
					
				if( focus->click_draggable() ) {
					set_drag_mode( focus );
					return false;
				}

				if( focus->picked( Input::MouseX(), Input::MouseY() ) ) {
				
					return true;
				}
			
			}
		}
		
	} else {
		gui_object *o = pick_object( );
		if( o != NULL && o != focus ) {
			if( o->get_accept_dnd() )
				drag_drop( o );
		}
	}
	return false;
	
	//	gui_object *o = pick_object( );
	//	drag_drop( o );
	//}
}

void gui::windows_message( UINT msg, WPARAM wParam, LPARAM lParam) {

	bool forward_message=false;
	switch( msg ) {
	case WM_LBUTTONDOWN:
		on_mousedown();
		forward_message=true;
		break;
	case WM_MOUSEMOVE:
		forward_message = on_mousemove();
		
		break;
	case WM_LBUTTONUP:
		forward_message  = on_mouseup();
		break;
	case WM_KEYDOWN:
		if( wParam== VK_TAB ) {
			if( focus ) {
				// todo, tab switch

			}
			break;
		}
		
	case WM_KEYUP:
	case WM_CHAR:
	case WM_RBUTTONDOWN:
		if( !drag_mode ) {
			forward_message=true;
		} else {
			cancel_drag();
		}

		break;
	case WM_RBUTTONUP:
		
		if( focus ) {
			if( focusheld ) {
				if( focus->picked( Input::MouseX(), Input::MouseY() ) ) {
						
					forward_message=true;
						
				}

				
			}
		}
	}

	if( focus && forward_message ) {
		focus->windows_message( msg, wParam, lParam );
	}
}
/*
void gui::keyboard_input() {
	if( Input::PeekCharBuffer() ){ 
		if( focus ) {
			focus->raise_event( EVENT_KEYINPUT );
		}
	}
}*/

void gui::set_cursor( int item ) {
	if( cursor != item ) {
		cursor = item;
		ShowCursor( cursor == 0 );
	}
}
 
void gui::draw() {
	for( u32 i = 0; i < pages.size(); i++ ) {
		if( pages[i] ) {
			pages[i]->draw();
		}
	}
	if( cursor > 0 ) {
		cml::vector2f m;
		m = Input::GetMouse2f();
		set_vertex_texture( Textures::ITEMS );
		set_vertex_color(255,255,255,255);
		float u1,v1,u2,v2;
		get_item_icon_uv( cursor,u1,v1,u2,v2);
		add_rect( m[0]-15, m[1]-15, 31,31, u1, v1, u2, v2 );
	}
}

bool gui_initialized=false;

void windows_message( UINT msg, WPARAM wParam, LPARAM lParam) {
	if( gui_initialized ) {

		// FILTER MESSAGE
		switch( msg ) {
		case WM_CHAR:
		case WM_KEYDOWN:
		case WM_KEYUP:
		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_MOUSEMOVE:
		case WM_RBUTTONDOWN:
		case WM_RBUTTONUP:
		case WM_MBUTTONDOWN:
		case WM_MBUTTONUP:

			GUI.windows_message( msg, wParam, lParam);
		}
		
	}
}
/*
void init_gui() {

	
	gui_initialized=true;
	// title screen ------------
	gp_title.add( &img_title_logo );
	gp_title.add( &btn_title_login );
	gp_title.add( &btn_title_options );
	gp_title.add( &btn_title_exit );
	gp_title.add( &txt_title_server );
	gp_title.add( &txt_title_name );
	gp_title.add( &txt_title_password );

	gp_title.add( &lbl_title_server );
	gp_title.add( &lbl_title_name );
	gp_title.add( &lbl_title_password );

	img_title_logo.set_image( Textures::ARCHUBOS_LOGO, hud_color(255,255,255,255), 0, 0, 1, 1 );
	img_title_logo.set_rect( 0, 100, 1024, 256 );
	img_title_logo.set_anchor( NULL, HALIGN_CENTER, VALIGN_TOP_INSIDE );
	 
	btn_title_login.set_caption( "Login" );
	btn_title_options.set_caption( "Options"  );
	btn_title_exit.set_caption( "Exit" );

	btn_title_options.set_anchor( &btn_title_login, HALIGN_CENTER, VALIGN_BOTTOM_OUTSIDE );
	btn_title_exit.set_anchor( &btn_title_options, HALIGN_CENTER, VALIGN_BOTTOM_OUTSIDE );

	btn_title_login.set_rect( 0, 200, 500, 80 );
	btn_title_options.set_rect( 0,  0, 500, 80 );
	btn_title_exit.set_rect( 0,  64, 500, 80 );

	btn_title_exit.hook( TitleScreen::OnExitButton );

	//txt_title_server.set_caption();
	txt_title_server.set_rect( 0,-100, 500, 50 );
	txt_title_name.set_rect(0,32,500,50);
	txt_title_password.set_rect(0,32,500,50);

	txt_title_name.set_anchor( &txt_title_server, HALIGN_CENTER, VALIGN_BOTTOM_OUTSIDE );
	txt_title_password.set_anchor( &txt_title_name, HALIGN_CENTER, VALIGN_BOTTOM_OUTSIDE );

	lbl_title_server.set_caption( "Server Address" );
	lbl_title_server.set_position( 4, -4 );
	lbl_title_server.set_anchor( &txt_title_server, HALIGN_LEFT_INSIDE, VALIGN_TOP_OUTSIDE );

	
	lbl_title_name.set_caption( "Your Name" );
	lbl_title_name.set_position( 4, -4 );
	lbl_title_name.set_anchor( &txt_title_name, HALIGN_LEFT_INSIDE, VALIGN_TOP_OUTSIDE );

	
	lbl_title_password.set_caption( "Your Password" );
	lbl_title_password.set_position( 4, -4 );
	lbl_title_password.set_anchor( &txt_title_password, HALIGN_LEFT_INSIDE, VALIGN_TOP_OUTSIDE );

	//-------------------------------------------------------------------------------------------
	for( int x = 0; x < 8; x++ ) {
		for( int y = 0; y < 8; y++ ) {
			gui_slot *s = testslot+x+y*8;
			gp_test.add(s);
			
			s->set_item(3 + rand() % 6);
			s->set_rect(64 + x * 100, 64 + y * 100, 25 + y * 8, 25 + y * 8 );
			s->set_anchor( NULL, ALIGN_LEFT_INSIDE, ALIGN_TOP_INSIDE );
			s->set_draggable( true);
			s->set_click_draggable(true);
			s->hook(TitleScreen::pooper);
			s->set_accept_dnd(true);
		}
	}

	for( int i = 0; i < 3; i++ ) {
		gui_textbox *t = &test_text[i];
		gp_test.add( t );

		t->set_rect( 0, 0, 100 + i * 50, 48 + i * 5 );
		t->set_anchor( i == 0 ? NULL : &test_text[i-1], ALIGN_CENTER, i == 0 ? ALIGN_CENTER : ALIGN_BOTTOM_OUTSIDE );
	}

	test_text[0].set_rect( 100,-250,100,48);
	test_text[2].set_font( gui_font( 20, 2 ) );

	for( int x = 0; x < 4; x++ ) {
		for( int y = 0; y < 4; y++ ) {
			gui_pushbutton *t = &test_buttons[x+y*4];
			gp_test.add( t );
			char caption[2];
			caption[1] = 0;
			caption[0] = 'a' + x+y*4;
			
			t->set_caption( caption );
			t->set_rect( -400 + x * 60, 300 + y * 60, 50,50 );
			t->set_anchor( NULL , ALIGN_RIGHT_INSIDE, ALIGN_TOP_INSIDE );
			
		}}

	test_exit_button.set_anchor( &test_buttons[12], ALIGN_LEFT_INSIDE, ALIGN_BOTTOM_OUTSIDE );
	test_exit_button.set_rect( 0,100,200,150 );
	test_exit_button.set_caption( "EXIT" );
	test_exit_button.set_font( gui_font(20,2));
	test_exit_button.hook( TitleScreen::OnExitButton );
	gp_test.add(&test_exit_button);

	for( int i = 0; i < 3; i++ ) {
		gui_bar *a = &test_bar[i];
		//a->create();
		if( i != 0 ) a->set_anchor( &test_bar[i-1], ALIGN_CENTER, ALIGN_BOTTOM_OUTSIDE );
		a->set_rect( 0,0, 150 + i * 20, 14 + i * 5 );
			
		gp_test.add(a );
	}
	test_bar[0].set_anchor( &test_text[2], ALIGN_CENTER, ALIGN_BOTTOM_OUTSIDE );
	test_bar[0].set_color( hud_color(255,0,0) );
	test_bar[1].set_color( hud_color(255,255,0) );
	test_bar[2].set_color( hud_color(32,112,245) );

	test_scroller.set_rect( 0,0,222,32 );
	test_scroller.set_anchor( &test_bar[2], ALIGN_CENTER, ALIGN_BOTTOM_OUTSIDE );
	gp_test.add(&test_scroller);

	
	test_scroller2.set_rect( 0,0,26,224 );
	test_scroller2.set_anchor( &test_scroller, ALIGN_CENTER, ALIGN_BOTTOM_OUTSIDE );
	test_scroller2.set_vertical(true);
	gp_test.add(&test_scroller2);

	test_image.set_image( Textures::SKY, hud_color(), 0,0,1,1);
	test_image.set_rect( 1205, 50, 200,200);
	test_image.set_anchor( NULL, ALIGN_LEFT_INSIDE, ALIGN_TOP_INSIDE );
	gp_test.add(&test_image);

	test_label.set_rect( 0,0,0,32);
	test_label.set_caption( "TEST label, anchored below image" );
	test_label.set_anchor( &test_image, ALIGN_LEFT_INSIDE, ALIGN_BOTTOM_OUTSIDE );
	gp_test.add( &test_label );

	// main -------------
	GUI.clear();
	//GUI.add_page( &gp_title );
	//GUI.add_page( &gp_test) ;
}*/

}

#endif
