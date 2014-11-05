//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

#include "stdafx.h"
#include "system/server/serverconsole.h"
#include "system/console.h"
#include "system/system.h"
 
namespace System { namespace ServerConsole {
	
//-----------------------------------------------------------------------------
enum {
	WINDOW_OUTPUT,
	WINDOW_HEADER,
	WINDOW_SIDEBAR,
	WINDOW_INPUT,
	WINDOW_COUNT
};

//-----------------------------------------------------------------------------
WINDOW *windows[WINDOW_COUNT];
PANEL *panels[WINDOW_COUNT];

boost::mutex console_mutex;

//-----------------------------------------------------------------------------
void SetTitle( const char *text ) {

	{
		boost::lock_guard<boost::mutex> lock(console_mutex);
		mvwaddch( windows[WINDOW_HEADER], 0,0,' ' );
		for( int i = 0; i < 80; i++ )
			waddch( windows[WINDOW_HEADER], ' ');
		mvwprintw( windows[WINDOW_HEADER], 0,1, text );
	}
}

//-----------------------------------------------------------------------------
void InitializeWindows() {
		
	// 80X24
	windows[WINDOW_OUTPUT] = newwin( 22, 60, 1,0 ); 
	windows[WINDOW_SIDEBAR] = newwin( 22,20, 1,60 );
	windows[WINDOW_INPUT] = newwin( 1,80, 23,0 );
	windows[WINDOW_HEADER] = newwin( 1, 80, 0,0 );
	for( int i = 0; i < (sizeof windows)/sizeof(WINDOW*); i++ ) {
		panels[i] = new_panel( windows[i] );
		
	}

	wbkgd( windows[WINDOW_HEADER], COLOR_PAIR(1) );
	wbkgd( windows[WINDOW_OUTPUT], COLOR_PAIR(2) );
	wbkgd( windows[WINDOW_SIDEBAR], COLOR_PAIR(3) );

	scrollok( windows[WINDOW_OUTPUT], true );
	
	update_panels();
}




//-----------------------------------------------------------------------------
void ClearInputWindow() {
	werase( windows[WINDOW_INPUT] );
}

//-----------------------------------------------------------------------------
void GetInputEx( const char *prompt, char *input, int maxlen ) {
	{
		boost::lock_guard<boost::mutex> lock(console_mutex);
		mvwprintw( windows[WINDOW_INPUT], 0, 0, prompt );
	}
	wgetnstr( windows[WINDOW_INPUT], input, maxlen );
	{
		boost::lock_guard<boost::mutex> lock(console_mutex);
		ClearInputWindow();
	}
}

//-----------------------------------------------------------------------------
void GetInput( char *input, int maxlen ) {
	GetInputEx( "$ ", input, maxlen );
}

//-----------------------------------------------------------------------------
void PrintToWindow( const char * text, bool newline ) {

	boost::lock_guard<boost::mutex> lock(console_mutex);
	 
	if( newline ) wprintw( windows[WINDOW_OUTPUT], "\n" );
	wprintw( windows[WINDOW_OUTPUT], "%s", text );
     
	// todo OPTIMIZE
	update_panels();
	doupdate();
	
}

//-----------------------------------------------------------------------------
void PrintToWindow( const char * format, bool newline, va_list args ) {

	boost::lock_guard<boost::mutex> lock(console_mutex);
	 
	if( newline ) wprintw( windows[WINDOW_OUTPUT], "\n" );
	vwprintw( windows[WINDOW_OUTPUT], format, args );

	// todo OPTIMIZE
	update_panels();
	doupdate();

	
}
/*
//-------------------------------------------------------------------------------------------------
void Print( const char *format, ... ) {
	va_list argptr;
	va_start(argptr, format);
	PrintToWindow( format, true, true, argptr );
	va_end(argptr);
} 

void Print( const char *text ) {

}

//-------------------------------------------------------------------------------------------------
void PrintEx( const char *format, ... ) {
	va_list argptr;
	va_start(argptr, format);
	PrintToWindow( format, false, false, argptr );
	va_end(argptr);
}*/

//-------------------------------------------------------------------------------------------------
void SetMenuItem( int line, const char *format, bool update, ... ) {
	if( line < 0 || line >= SIDEBAR_HEIGHT ) return;
	boost::lock_guard<boost::mutex> lock(console_mutex);
	va_list argptr;
    va_start(argptr, update);
	char buffer[32];

	for( int i = 0; i < SIDEBAR_WIDTH; i++ ) 
		buffer[i] = ' ';
	buffer[SIDEBAR_WIDTH] = 0;
	mvwaddstr( windows[WINDOW_SIDEBAR], line, 0, buffer );

	vsnprintf( buffer, sizeof buffer, format, argptr );
	va_end(argptr);
	
	mvwprintw( windows[WINDOW_SIDEBAR], line, 0, buffer );
	if( update ) {
		update_panels();
		doupdate();
	}
}


//-------------------------------------------------------------------------------------------------
void Update() {
	boost::lock_guard<boost::mutex> lock(console_mutex);
	update_panels();
	doupdate();
}

//-------------------------------------------------------------------------------------------------
void IOThread() {
	char input[256];
	while( System::Live() ) {
		System::ServerConsole::GetInput( input, sizeof input );
		if( !System::Live() ) break;
		System::Console::Execute( input );

		System::ServerConsole::Update();
	}
}

Init::Init( const std::string &window_title ) {
	initscr();
	start_color();
	init_pair(3, COLOR_BLACK , COLOR_WHITE );
	init_pair(2, COLOR_WHITE+8 , COLOR_CYAN );
	init_pair(1, COLOR_WHITE+8, COLOR_BLUE+8);

	keypad(stdscr, TRUE);

	refresh();

	InitializeWindows();

	ClearInputWindow();

	SetTitle( window_title.c_str() );
}

Init::~Init() {
	endwin();
}


}}
