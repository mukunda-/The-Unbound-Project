//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

#include "stdafx.h"
#include "system/server/serverconsole.h"
#include "linereader.h"
#include "system/console.h"

namespace System { namespace ServerConsole {

//-----------------------------------------------------------------------------
LineReader::LineReader( WINDOW  *window ) {
	m_cursor = 0;
	m_length = 0;
	m_history_index = 0;
	m_window = nullptr;
	m_view = 0;

	if( window != nullptr ) {
		SetWindow( window );
	}
}

void LineReader::SetWindow( WINDOW *window ) {
	m_window = window;

	int dummy;
	getmaxyx( m_window, m_height, m_width );
}

//-----------------------------------------------------------------------------
void LineReader::MoveCursor( int pos ) {
	if( pos > m_length ) pos = m_length;
	if( pos >= INPUT_LENGTH ) pos = INPUT_LENGTH-1;
	if( pos < 0 ) pos = 0;

	m_cursor = pos;
	if( m_cursor <= m_view ) {
		if( m_view != 0 ) {
			m_view = m_cursor -10;
			if( m_view < 0 ) m_view = 0;
			m_dirty = true;
		}
	}
	if( m_cursor >= (m_view+m_width) ) {
		m_view = m_cursor - m_width + 1;
		if( m_view < 0 ) m_view = 0;
		m_dirty = true;
	}

}

//-----------------------------------------------------------------------------
bool LineReader::CursorOnScreen() {
	return m_cursor >= m_view && m_cursor < (m_view+m_width);
}

//-----------------------------------------------------------------------------
void LineReader::Redraw() {
	if( !m_dirty ) return;
	ConsoleLock lock;

	int x;
	for( x = 0; x < m_width; x++ ) {
		if( m_view + x >= m_length ) break;
		mvwaddch( m_window, 0, x, m_buffer[ m_view + x ] );
	}

	for( ; x < m_width; x++ ) {
		mvwaddch( m_window, 0, x, ' ' );
	}
	m_dirty = false;
}

//-----------------------------------------------------------------------------
void LineReader::DrawCursorChar( char c ) {
	if( m_cursor >= m_view && m_cursor < m_view + m_width ) {
		mvwaddch( m_window, 0, m_cursor-m_view, c );
	}
}

//-----------------------------------------------------------------------------
void LineReader::InputChar( int c ) {
	
	if( c >= 32 && c < 127 ) {
		if( m_length == INPUT_LENGTH ) return;

		if( m_cursor == m_length ) {
			// append
		} else {
			// insert
			m_dirty = true;
			for( int i = m_length; i >= m_cursor+1; i-- ) {
				m_buffer[i] = m_buffer[i-1];
			}
		}
		
		m_buffer[m_cursor] = c;
		DrawCursorChar(c);
		m_length++;
		MoveCursor(m_cursor+1);
		Redraw();
		 
	} else {
		if( c == 10 || c == 13 ) {
			// submit input.

		} else if( c == 8 || c == KEY_BACKSPACE ) {
			if( m_cursor == 0 ) return;
			for( int i = m_cursor; i < m_length; i++ ) {
				m_buffer[i-1] = m_buffer[i];
			}
			m_dirty = true;
			m_length--;
			MoveCursor(m_cursor-1);
			Redraw();
			// backspace
		} else if( c == 127 || c == KEY_DC ) {
			// delete
			if( m_cursor == m_length ) return;
			for( int i = m_cursor; i < m_length-1; i++ ) {
				m_buffer[i] = m_buffer[i+1];
			}
			m_dirty = true;
			m_length--;
			Redraw();
		} else if( c == KEY_LEFT ) {
			MoveCursor( m_cursor-1 );
		} else if( c == KEY_RIGHT ) {
			MoveCursor( m_cursor+1 );
		} else if( c == KEY_UP ) {
			// previous history
		} else if( c == KEY_DOWN ) {
			// next history
			
		} else if( c == KEY_HOME ) {
			MoveCursor(0);
		} else if( c == KEY_END ) {
			MoveCursor( m_length );
		}
	}
}

//void LineReader::PushHistory( const char *text ) {

//}

void LineReader::Process() {
	MoveCursor( m_cursor );
	int input = mvwgetch( m_window, 0, m_cursor-m_view );
	InputChar( input );
	System::Console::Print( "%d", input );
}

LineReader::HistoryEntry::HistoryEntry( const char *text ) {

}

//-----------------------------------------------------------------------------
}}