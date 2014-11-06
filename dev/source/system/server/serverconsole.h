//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//

// ServerConsole
//
// thread safe PDCURSES console interface and hardcoded layout
//

#pragma once
//-------------------------------------------------------------------------------------------------
 
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
void PrintToWindow( const char * text, bool newline  );

/// ---------------------------------------------------------------------------
/// Read a string from the input box.
///
/// @param input  Buffer to store input.
/// @param maxlen Size of buffer.
///
void GetInput( char *input, int maxlen );

/// ---------------------------------------------------------------------------
/// Read a string from the input box.
///
/// @param prompt Prompt to display on the left side of the input box.
/// @param input  Input storage buffer.
/// @param maxlen Size of input buffer.
///
void GetInputEx( const char *prompt, char *input, int maxlen );

/// ---------------------------------------------------------------------------
/// Redraw the windows.
///
void Update();

/// ---------------------------------------------------------------------------
/// Set a line on the menu box (sidebar).
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
struct Init {
	
	/// -----------------------------------------------------------------------
	/// Initialize the server console.
	///
	/// @param title Title of program, may be changed later with SetTitle()
	///
	Init( const std::string &title = "" );
	~Init();

public:

#if BOOST_ASIO_HAS_POSIX_STREAM_DESCRIPTOR
//	boost::asio::posix::stream_descriptor m_input_stream;
#else
	//boost::asio::windows::stream_handle m_input_stream;
#endif
	//boost::asio::streambuf m_input_buffer;

};

//------------------------------------------------------------------------------
}} // System::ServerConsole
