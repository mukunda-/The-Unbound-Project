//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//

// ServerConsole
//
// thread safe PDCURSES console interface and hardcoded layout
//

#pragma once
//-------------------------------------------------------------------------------------------------

#include "system/server/linereader.h"
 
namespace System {
namespace ServerConsole {

enum {
	WINDOW_OUTPUT=0,
	WINDOW_HEADER,
	WINDOW_SIDEBAR,
	WINDOW_INPUT,
	WINDOW_COUNT,
};

enum {
	SIDEBAR_WIDTH = 20,
	SIDEBAR_HEIGHT = 22
};

/// ---------------------------------------------------------------------------
/// Change the title bar of the server console window.
///
/// @param text Text to display, should be under 60 chars.
///
void SetTitle( const char *text );

/// ---------------------------------------------------------------------------
/// Print data to the output window.
///
/// @param format  printf format string.
/// @param newline if true, append a newline to the end of the text
/// @param args    Arguments to be formatted
///
void PrintToWindow( const char * format, bool newline, va_list args );

/// ---------------------------------------------------------------------------
/// Print simple data to the output window.
///
/// @param text    Text to print
/// @param newline if true, append a newline to the text
///
void PrintToWindow( const char * text, bool newline );

/// ---------------------------------------------------------------------------
/// Redraw the windows.
///
void Update();

/// ---------------------------------------------------------------------------
/// Set a line in the menu box (sidebar).
/// 
/// @param line   Number of line to change 0..21
/// @param format printf format string.
/// @param update call Update() afterwards
/// @param ...    Arguments to format.
///
void SetMenuItem( int line, const char *format, bool update=true, ... );

/// ---------------------------------------------------------------------------
/// Get a curses window handle from its index
///
/// @param index WINDOW_*
/// @returns Window handle for use with curses functions.
///
WINDOW *GetWindowHandle( int index );

/// ---------------------------------------------------------------------------
/// Initialization struct
///
struct Instance {
	friend class ConsoleLock;
	
	/// -----------------------------------------------------------------------
	/// Initialize the server console.
	///
	/// @param title Title of program, may be changed later with SetTitle()
	///
	Instance( const std::string &title = "" );
	~Instance();

	void SetTitle( const char *text );
	void PrintToWindow( const char * text, bool newline );
	void PrintToWindow( const char * format, bool newline, va_list args );
	void SetMenuItem( int line, const char *format, bool update, va_list args );
	void Update();
	WINDOW *GetWindowHandle( int index );
	
	

private:
	WINDOW *m_windows[WINDOW_COUNT];
	PANEL *m_panels[WINDOW_COUNT];
	LineReader m_linereader;
	boost::mutex m_mutex;
	std::thread m_iothread;

	HANDLE m_terminate_event;

	// windows virtual-key mapping to curses
	int m_vkey_map[256];

	//std::unordered_map<int,int> m_windows_curses_map;

	void InitializeWindows();
	void ClearInputWindow();
	void InputChar( char c );
	void IOThread();
	void HandleConsoleInput();
	
};

/// -----------------------------------------------------------------------
/// This class must be instantiated during any call 
/// to the curses functions.
///
class ConsoleLock {

	boost::lock_guard<boost::mutex> lock;
public:
	ConsoleLock( Instance *parent = nullptr );
};


//-----------------------------------------------------------------------------
}} // System::ServerConsole
