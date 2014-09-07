//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//

#pragma once

#ifndef HUD_H
#define HUD_H

namespace hud {

enum {
	MODE_LIVE,
	MODE_TITLE,
	MODE_TITLE_SERVER,
	MODE_ERROR,
	MODE_OPTIONS,
	MODE_INVENTORY,
	MODE_CHAT
};


enum {
	HALIGN_LEFT_OUTSIDE=0,
	HALIGN_LEFT_CENTER=1,
	HALIGN_LEFT_INSIDE=2,
	HALIGN_CENTER_LEFT=4,
	HALIGN_CENTER=5,
	HALIGN_CENTER_RIGHT=6,
	HALIGN_RIGHT_INSIDE=8,
	HALIGN_RIGHT_CENTER=9,
	HALIGN_RIGHT_OUTSIDE=10,

	VALIGN_TOP_OUTSIDE=0,
	VALIGN_TOP_CENTER=1,
	VALIGN_TOP_INSIDE=2,
	VALIGN_CENTER_LEFT=4,
	VALIGN_CENTER=5,
	VALIGN_CENTER_RIGHT=6,
	VALIGN_BOTTOM_INSIDE=8,
	VALIGN_BOTTOM_CENTER=9,
	VALIGN_BOTTOM_OUTSIDE=10,

	ALIGN_TOP_OUTSIDE=0,
	ALIGN_TOP_CENTER=1,
	ALIGN_TOP_INSIDE=2,
	ALIGN_LEFT_OUTSIDE=0,
	ALIGN_LEFT_CENTER=1,
	ALIGN_LEFT_INSIDE=2,
	ALIGN_CENTER_LEFT=4,
	ALIGN_CENTER=5,
	ALIGN_CENTER_RIGHT=6,
	ALIGN_BOTTOM_INSIDE=8,
	ALIGN_BOTTOM_CENTER=9,
	ALIGN_BOTTOM_OUTSIDE=10,
	ALIGN_RIGHT_INSIDE=8,	
	ALIGN_RIGHT_CENTER=9,
	ALIGN_RIGHT_OUTSIDE=10,

	//		
	//       LEFT			 CENTER				RIGHT
	// OUTSIDE|INSIDE		LEFT|RIGHT		INSIDE|OUTSIDE
//	        CENTER			  CENTER			CENTER

	//
};


typedef enum {
	// user messages
	USER_MSG_DROP,
	USER_MSG_NULLDRAG
};

/*
enum {
	EVENT_CLICKED,
	EVENT_KEYINPUT 
};
*/


class hud_color {
public:
	u8 r, g, b, a;

	hud_color() {r=255; g=255; b=255; a=255;}
	hud_color( u8 pr, u8 pg, u8 pb ) { r=pr; g=pg; b=pb; a=255; }
	hud_color( u8 pr, u8 pg, u8 pb, u8 pa ) { r=pr; g=pg; b=pb; a=pa; }
};

void init();

// add square primitive
//void add( float x, float y, float w, float h, float sx, float sy, float lum );

void use_font( RasterFonts::Font *source );
void draw_text( int x, int y, const char *text );
void draw_bar( int x1, int y, int width, float filled, int type );
//void draw_button( float x, float y, float width, float height );

void windows_message( UINT msg, WPARAM wParam, LPARAM lParam);

void set_mode( int newmode );
int get_mode();
//cml::vector2f get_mouse();

void update();
void draw();

// render result
void render();
 

typedef void (*gui_message)( void *source, UINT msg, WPARAM wParam, LPARAM lParam);
//--------------------------------------------------------------------------------------------------------
class gui_font  {
//--------------------------------------------------------------------------------------------------------
	int height;
	int stroke;
	bool global;
public:
	gui_font( int h ) { global=false; height = h; stroke = 0; };
	gui_font( int h, int s ) { global=false; height = h; stroke = s; };
	gui_font() { global=true; }

	bool get_params( int &h, int &s ) const { h = height; s = stroke; return !global; }
};

//--------------------------------------------------------------------------------------------------------
class gui_object {
//--------------------------------------------------------------------------------------------------------

protected:
	float x, y, width, height;

	bool mouseover;
	bool focused;	
	bool pressed;
	bool allocated;
	bool centered;
	bool hot;
	bool dragging;
	bool held;

	gui_font font;
	
	bool is_draggable;
	bool is_click_draggable;
	bool accept_dnd;

	int h_align;
	int v_align;

	void *user_data;
	int user_data_tag;
	bool user_data_needs_to_be_deleted;

	float h_padding;
	float v_padding;

	gui_message handler;

	gui_object *anchor;
	
	void translate( float &dx, float &dy );
	void translate_rect( float &dx1, float &dy1, float &dx2, float &dy2 );

	int item;

public:
	gui_object();
	~gui_object();

	void set_font( gui_font &f ) {font = f;}

	virtual void draw();
//	void set_rect( float X, float Y, float WIDTH, float HEIGHT, bool CENTERED );
	void set_rect( float X, float Y, float WIDTH, float HEIGHT );
	//void set_alignment( int h, int v );

	void setpressed( bool p ) { pressed = p; }
	void setheld( bool h ) { held = h; }

	bool picked( float px, float py );
	
	bool delete_on_exit() { return allocated; }

	void hook( gui_message h ) { handler = h; }

	void setfocus( bool f ) { focused = f; }
	void sethot( bool hawt ) { hot = hawt; }
	void set_anchor( gui_object *p_parent, int p_h_align, int p_v_align );
	void set_draggable( bool a ) { is_draggable = a; }
	void set_click_draggable( bool a ) { is_click_draggable = a; }
	bool draggable() { return is_draggable && (item != 0); }
	bool click_draggable() { return is_click_draggable && (item != 0); }
	void set_dragging(bool a) { dragging=a; }
	void set_accept_dnd( bool a ) { accept_dnd=a; }
	bool get_accept_dnd() { return accept_dnd; }

	void set_user_data( void *data, int tag, bool autofree );
	int  get_user_data( void **data );

	void set_item( int );
	int  get_item();
	void swap_item( gui_object & );
	void remove_item();

	//virtual void raise_event( int msg );

	void set_position( float X, float Y );

	float get_width() { return width; }
	float get_height() { return height; }
	
	virtual void windows_message( UINT msg, WPARAM wParam, LPARAM lParam);

	void drop( gui_object *source );
	void nulldrag();
};

//--------------------------------------------------------------------------------------------------------
class gui_frame : public gui_object {
//--------------------------------------------------------------------------------------------------------

public:

};

//--------------------------------------------------------------------------------------------------------
class gui_slot : public gui_object {
//--------------------------------------------------------------------------------------------------------
	

public:
	

	void draw();
};

//--------------------------------------------------------------------------------------------------------
class gui_scroller : public gui_object {
//--------------------------------------------------------------------------------------------------------
	float value;
	bool vertical;
	
	float get_thumbsize();
	void mouse_position( float mx, float my );
	void get_thumb_min_max( float &min, float &max );

public:
	gui_scroller():gui_object() { 
		value = 0.5;
		vertical=false;
	};
	void set_value( float v );
	float get_value();
	void set_vertical( bool v );
	void windows_message( UINT msg, WPARAM wParam, LPARAM lParam);
	void draw();
};

//--------------------------------------------------------------------------------------------------------
class gui_bar : public gui_object {
//--------------------------------------------------------------------------------------------------------

	float filled;
	hud_color color;

public:
	gui_bar():gui_object() {
		filled=0;

	}
	
	void set_color( const hud_color &pcolor ) { color = pcolor; }
	void set_filled( float pfilled );
	float get_filled();
	void draw();
};

//--------------------------------------------------------------------------------------------------------
class gui_label : public gui_object {
//--------------------------------------------------------------------------------------------------------
	std::string caption;
	hud_color text_color;

public:
  
	void set_color( const hud_color &ptext_color );
	void set_caption( const char *pcaption );
	void draw();
};

//--------------------------------------------------------------------------------------------------------
class gui_pushbutton : public gui_object {
//--------------------------------------------------------------------------------------------------------
	std::string caption;
	hud_color text_color;
public:
	void set_caption( const char *pcaption );
	void set_textcolor( const hud_color &col );
	void draw();
};

//--------------------------------------------------------------------------------------------------------
class gui_textbox : public gui_object {
//--------------------------------------------------------------------------------------------------------
	std::string text;
	float scroll;
	int caret;

	double caret_blink;

	void reset_caret_blink();

public:
	gui_textbox():gui_object() {
		scroll=0;
		caret=0;
		caret_blink=0;

	}
	
	void draw();

	//void raise_event( int msg );

	void scroll_to_caret();

	void set_caret_position_pixels( int px );
	
	void windows_message( UINT msg, WPARAM wParam, LPARAM lParam);
	
};

//--------------------------------------------------------------------------------------------------------
class gui_image : public gui_object {
//--------------------------------------------------------------------------------------------------------
	
	float u1, v1, u2, v2;
	unsigned int texture;
	hud_color color;

public:
	gui_image():gui_object() {
		u1=v1=u2=v2=0;
		texture=0;
	}
	void set_image( unsigned int texture, const hud_color &col, float u1, float v1, float u2, float v2 );
	void draw();
};

//--------------------------------------------------------------------------------------------------------
class gui_page {
//--------------------------------------------------------------------------------------------------------
	std::vector<gui_object*> objects;

	gui_page *link;
public:
	gui_page();
	~gui_page();

	void add( gui_object * );
	void draw();
	gui_object * pick( float x, float y );
};

//--------------------------------------------------------------------------------------------------------
class gui { 
//--------------------------------------------------------------------------------------------------------

	std::vector<gui_page*> pages;

	bool focusheld;
	bool drag_mode;

	cml::vector2f mousedownpos;
	gui_object *focus;
	gui_object *hot;

	gui_object *drag_source;

	int cursor;
	
	void on_mousedown();
	bool on_mousemove();
	bool on_mouseup();

	gui_object * pick_object();

public:
	gui();
	void clear();
	void add_page( gui_page * );

	void set_hot( gui_object * );

	void release_focus();

	void draw();

	void set_cursor( int item );
	void set_drag_mode( gui_object *source );
	void cancel_drag();
	void drag_drop( gui_object *target );
	
	void windows_message( UINT msg, WPARAM wParam, LPARAM lParam);
};

};

#endif
