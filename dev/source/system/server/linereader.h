//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

#pragma once

#include "util/linkedlist.h"

namespace System { namespace ServerConsole {

class LineReader {

	class HistoryEntry : public Util::LinkedItem<HistoryEntry> {
		std::unique_ptr<char> text;
	public:
		HistoryEntry( const char *text );
	};

	Util::LinkedList<HistoryEntry> m_history;
	 
//	char m_history[HISTORY_SIZE][INPUT_LENGTH]; // history of completed lines
//	int  m_history_index; // navigation index when pressing up/down
	std::string m_buffer;
	//char m_buffer[INPUT_LENGTH]; // current line being written to
	int  m_cursor; // text caret position
	int  m_length; // length of current buffered line
	int  m_view; // position of leftmost char, the horizontal scrolling offset
	bool m_dirty; // needs full redraw

	int  m_width; // width of window
	int  m_height; // height of window (not used)
	WINDOW *m_window; // window to paint to 

	bool CursorOnScreen();
	void Redraw();
	void DrawCursorChar( char c );
	
public:

	/// -----------------------------------------------------------------------
	/// Construct the LineReader.
	///
	/// There should only be one instance of this.
	///
	/// @param window Window to edit while typing. May be specified later with
	///               SetWindow().
	///
	LineReader( WINDOW* window = nullptr );

	/// -----------------------------------------------------------------------
	/// Input a character from the user
	///
	/// @param c Character to input.
	///
	void InputChar( int c );

	void MoveCursor( int pos );

	void SetWindow( WINDOW *window );

	/// --------------------------------------------------------------------------
	/// Read a character from the user and input it.
	///
	void Process();
};

}}