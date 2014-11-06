//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

#include "stdafx.h"
#include "system/server/serverconsole.h"
#include "system/console.h"
#include "system/system.h"
 
namespace System { namespace ServerConsole {
	
//-----------------------------------------------------------------------------
Init *g_instance;

//-----------------------------------------------------------------------------
WINDOW *windows[WINDOW_COUNT];
PANEL *panels[WINDOW_COUNT];

#define HISTORY_SIZE 128
#define INPUT_LENGTH 1024

char g_history[HISTORY_SIZE][INPUT_LENGTH];
int g_history_position;

char g_input_buffer[INPUT_LENGTH]; 
int g_cursor; // position in input buffer
int g_input_length;

char g_stdin_data[8];

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
//----------------------------------------------------------------------------
void Print( const char *format, ... ) {
	va_list argptr;
	va_start(argptr, format);
	PrintToWindow( format, true, true, argptr );
	va_end(argptr);
} 

void Print( const char *text ) {

}

//-----------------------------------------------------------------------------
void PrintEx( const char *format, ... ) {
	va_list argptr;
	va_start(argptr, format);
	PrintToWindow( format, false, false, argptr );
	va_end(argptr);
}*/

//-----------------------------------------------------------------------------
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


//-----------------------------------------------------------------------------
void Update() {
	boost::lock_guard<boost::mutex> lock(console_mutex);
	update_panels();
	doupdate();
}


//-----------------------------------------------------------------------------
WINDOW *GetWindowHandle( int index ) {
	return windows[index];
}

//-----------------------------------------------------------------------------
void InputChar( char c ) {

	if( c >= 32 && c < 127 ) {
		g_input_buffer[g_cursor] = c;
		if( g_cursor < g_input_length ) {
			g_cursor++;
		} else {
			g_input_buffer[g_cursor] = c;
			g_cursor++;
			g_input_length++;
		}
		if( g_cursor > INPUT_LENGTH ) {
			g_cursor = INPUT_LENGTH;
		}
	} else {
		if( c == '\n' ) {
			// submit input.
		} else if( c == 8 ) {
			// backspace
		} else if( c == 127 ) {
			// delete
		}
	}
}

//-----------------------------------------------------------------------------
void IOThread() {

	int a = getch();
	System::Console::Print( "%d", a );
	/*
	char input[256];
	while( System::Live() ) {
		System::ServerConsole::GetInput( input, sizeof input );
		if( !System::Live() ) break;
		System::Console::Execute( input );

		System::ServerConsole::Update();
	}*/
}
/*
void ReadInputStream();

//-----------------------------------------------------------------------------
void OnInput( boost::system::error_code error, int bytes_transferred ) {
	if( error ) {
		// break
	} else {
		// todo: do this right (lol..)
	//	std::istream is(&g_instance->m_input_buffer);
	//	std::string s;
	//	is >> s;
	//	InputChar( g_instance->m_input_buffer s.c_str()[0] );
		InputChar( g_stdin_data[0] );

		ReadInputStream();
	}
}*/
/*
//-----------------------------------------------------------------------------
void ReadInputStream() {
	
	g_instance->m_input_stream.async_read_some( 
			boost::asio::buffer( g_stdin_data, 1 ),
			boost::bind( 
				&OnInput, 
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred ) );
}*/

//-----------------------------------------------------------------------------
Init::Init( const std::string &window_title  )  {

	g_instance = this;

	initscr();
	start_color();
	init_pair(3, COLOR_BLACK , COLOR_WHITE );
	init_pair(2, COLOR_WHITE+8 , COLOR_CYAN );
	init_pair(1, COLOR_WHITE+8, COLOR_BLUE+8);
	cbreak();
	noecho();

	keypad(stdscr, TRUE);

	refresh();

	InitializeWindows();
	ClearInputWindow();
	SetTitle( window_title.c_str() );
	
	Update();

	g_cursor = 0;
	g_input_length = 0;
	g_history_position = 0;
	
	std::thread iothrad( IOThread );
	iothrad.detach();
	//ReadInputStream();
}

Init::~Init() {
	endwin();
}


}}
