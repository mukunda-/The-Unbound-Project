//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

#pragma once

#include "util/deletedlinkedlist.h"
#include "util/linebuffer.h"

namespace System { namespace ServerConsole {

class LineReader {
  
	Util::LineBuffer m_history;
	 
	std::string m_buffer; 
	int  m_cursor; // text caret position
	
	int  m_view; // position of leftmost char, the horizontal scrolling offset
	bool m_dirty; // needs full redraw

	int  m_width; // width of window
	int  m_height; // height of window (not used)
	WINDOW *m_window; // window to paint to 

	bool CursorOnScreen();
	void Redraw( bool force = false );
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
	/// Read a character from stdin and process it.
	///
	/// @returns false if EOF was read.
	///
	bool Process( int c );
};

}}