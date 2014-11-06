//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

#include "stdafx.h"
#include "system/server/serverconsole.h"
#include "system/console.h"
#include "system/system.h"
#include "system/server/linereader.h"
 
namespace System { namespace ServerConsole {
	
//-----------------------------------------------------------------------------
Instance *g_instance;
 
//-----------------------------------------------------------------------------
void Instance::SetTitle( const char *text ) {
	
	{
		ConsoleLock lock(this);
		mvwaddch( m_windows[WINDOW_HEADER], 0,0,' ' );
		for( int i = 0; i < 80; i++ )
			waddch( m_windows[WINDOW_HEADER], ' ');
		mvwprintw( m_windows[WINDOW_HEADER], 0,1, text );
	}
}

//-----------------------------------------------------------------------------
void Instance::InitializeWindows() {
	
	// 80X24
	m_windows[WINDOW_OUTPUT] = newwin( 22, 60, 1,0 ); 
	m_windows[WINDOW_SIDEBAR] = newwin( 22,20, 1,60 );
	m_windows[WINDOW_INPUT] = newwin( 1,80, 23,0 );
	m_windows[WINDOW_HEADER] = newwin( 1, 80, 0,0 );
	for( int i = 0; i < (sizeof m_windows)/sizeof(WINDOW*); i++ ) {
		m_panels[i] = new_panel( m_windows[i] );
		
	}

	wbkgd( m_windows[WINDOW_HEADER], COLOR_PAIR(1) );
	wbkgd( m_windows[WINDOW_OUTPUT], COLOR_PAIR(2) );
	wbkgd( m_windows[WINDOW_SIDEBAR], COLOR_PAIR(3) );

	scrollok( m_windows[WINDOW_OUTPUT], true );
	
	update_panels();
}

//-----------------------------------------------------------------------------
void Instance::ClearInputWindow() {
	werase( m_windows[WINDOW_INPUT] );
}
/*
//-----------------------------------------------------------------------------
void Instance::GetInputEx( const char *prompt, char *input, int maxlen ) {
	{
		boost::lock_guard<boost::mutex> lock(console_mutex);
		mvwprintw( windows[WINDOW_INPUT], 0, 0, prompt );
	}
	wgetnstr( windows[WINDOW_INPUT], input, maxlen );
	{
		boost::lock_guard<boost::mutex> lock(console_mutex);
		ClearInputWindow();
	}
}*/

/*
//-----------------------------------------------------------------------------
void GetInput( char *input, int maxlen ) {
	GetInputEx( "$ ", input, maxlen );
}*/

//-----------------------------------------------------------------------------
void Instance::PrintToWindow( const char * text, bool newline ) {
	ConsoleLock lock(this);
	 
	if( newline ) wprintw( m_windows[WINDOW_OUTPUT], "\n" );
	wprintw( m_windows[WINDOW_OUTPUT], "%s", text );
     
	// todo OPTIMIZE
	update_panels();
	doupdate();
	
}

//-----------------------------------------------------------------------------
void Instance::PrintToWindow( const char * format, bool newline, va_list args ) {
	ConsoleLock lock(this);
	 
	if( newline ) wprintw( m_windows[WINDOW_OUTPUT], "\n" );
	vwprintw( m_windows[WINDOW_OUTPUT], format, args );

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
void Instance::SetMenuItem( int line, const char *format, bool update, va_list argptr ) {
	if( line < 0 || line >= SIDEBAR_HEIGHT ) return;
	ConsoleLock lock(this);
	char buffer[32];

	for( int i = 0; i < SIDEBAR_WIDTH; i++ ) 
		buffer[i] = ' ';
	buffer[SIDEBAR_WIDTH] = 0;
	mvwaddstr( m_windows[WINDOW_SIDEBAR], line, 0, buffer );

	vsnprintf( buffer, sizeof buffer, format, argptr );
	
	mvwprintw( m_windows[WINDOW_SIDEBAR], line, 0, buffer );
	if( update ) {
		update_panels();
		doupdate();
	}
}


//-----------------------------------------------------------------------------
void Instance::Update() {
	ConsoleLock lock(this);
	update_panels();
	doupdate();
}


//-----------------------------------------------------------------------------
WINDOW *Instance::GetWindowHandle( int index ) {
	return m_windows[index];
}

//-----------------------------------------------------------------------------
void Instance::InputChar( char c ) {

}

//-----------------------------------------------------------------------------
void Instance::IOThread() {

	while( System::Live() ) {
		m_linereader.Process();	
	} 
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
Instance::Instance( const std::string &window_title ) {

	g_instance = this;

	initscr();
	start_color();
	init_pair( 3, COLOR_BLACK  , COLOR_WHITE  );
	init_pair( 2, COLOR_WHITE+8, COLOR_CYAN   );
	init_pair( 1, COLOR_WHITE+8, COLOR_BLUE+8 );
	cbreak();
	noecho();

	keypad( stdscr, TRUE );

	refresh();

	InitializeWindows();
	ClearInputWindow();
	SetTitle( window_title.c_str() );
	
	Update();
	
	keypad( m_windows[WINDOW_INPUT], TRUE );
	m_linereader.SetWindow( m_windows[ WINDOW_INPUT ] );

//	g_cursor = 0;
//	g_input_length = 0;
//	g_history_position = 0;
	
	std::thread iothread( std::bind( &Instance::IOThread, this ) );
	iothread.detach();
	//ReadInputStream();
}

Instance::~Instance() {
	endwin();

	g_instance = nullptr;
}

ConsoleLock::ConsoleLock( Instance *parent ) : 
	lock( parent ? parent->m_mutex : g_instance->m_mutex ) {}



//-----------------------------------------------------------------------------
// Easy access:
//-----------------------------------------------------------------------------

void SetTitle( const char *text ) {
	g_instance->SetTitle( text );
}

void PrintToWindow( const char * format, bool newline, va_list args ) {
	g_instance->PrintToWindow( format, newline, args );
}

void PrintToWindow( const char * text, bool newline  ) {
	g_instance->PrintToWindow( text, newline );
}
/*
void GetInput( char *input, int maxlen ) {
	g_instance->GetInput( input, maxlen );
}
void GetInputEx( const char *prompt, char *input, int maxlen ) {
	g_instance->
}*/

void Update() {
	g_instance->Update();
}

void SetMenuItem( int line, const char *format, bool update, ... ) {
	va_list argptr;
	va_start(argptr, format); 
	
	g_instance->SetMenuItem( line, format, update, argptr );
	va_end(argptr);
}

WINDOW *GetWindowHandle( int index ) {
	return g_instance->GetWindowHandle( index );
}

}}
