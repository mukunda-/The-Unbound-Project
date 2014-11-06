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
	m_window = nullptr;
	m_view = 0;
	m_history_iter = nullptr;

	if( window != nullptr ) {
		SetWindow( window );
	}
}

void LineReader::SetWindow( WINDOW *window ) {
	m_window = window; 
	getmaxyx( m_window, m_height, m_width );
}

//-----------------------------------------------------------------------------
void LineReader::MoveCursor( int pos ) {
	if( pos > (int)m_buffer.size() ) pos = (int)m_buffer.size();
	if( pos < 0 ) pos = 0;

	m_cursor = pos;
	if( m_cursor <= m_view ) {
		if( m_view != 0 ) {
			m_view = m_cursor - 10;
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
void LineReader::Redraw( bool force ) {
	if( !force && !m_dirty ) return;
	ConsoleLock lock;

	int x;
	for( x = 0; x < m_width; x++ ) {
		if( m_view + x >= (int)m_buffer.size() ) break;
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

		if( m_cursor == m_buffer.size() ) {
			// append
			m_buffer += (char)c;
			DrawCursorChar( c );
		} else {
			// insert
			m_dirty = true;
			 
			char chars[2];
			chars[1] = 0;
			chars[0] = c;
			m_buffer.insert( m_cursor, chars );
		}
		MoveCursor(m_cursor+1);
		Redraw();
	} else {
		if( c == 10 || c == 13 ) {
			// submit input.
			HistoryEntry *history = m_history_iter;
			if( !history ) history = m_history.GetLast();
			if( history ) {
				if( strcmp( history->Contents(), m_buffer.c_str() ) != 0 ) {
					
				}
			}
		} else if( c == 8 || c == KEY_BACKSPACE ) {
			if( m_cursor == 0 ) return;
			m_buffer.erase( m_cursor-1, 1 );
			MoveCursor(m_cursor-1);
			Redraw(true);
			// backspace
		} else if( c == 127 || c == KEY_DC ) {
			// delete
			if( m_cursor == m_buffer.size() ) return;
			m_buffer.erase( m_cursor, 1 );
			Redraw(true);
		} else if( c == KEY_LEFT ) {
			MoveCursor( m_cursor-1 );
			Redraw();
		} else if( c == KEY_RIGHT ) {
			MoveCursor( m_cursor+1 );
			Redraw();
		} else if( c == KEY_UP ) {
			// previous history
			if( m_history_iter == nullptr ) {
				m_history_iter = m_history.GetLast();
				if( !m_history_iter ) return;
				if( m_buffer.size() != 0 ) {
					m_history.Add( new HistoryEntry( m_buffer ) );
					m_history_counter++;
				}
			} else {
				if( !m_history_iter->m_prev ) return;
				m_history_iter = m_history_iter->m_prev;
			}
			m_buffer = m_history_iter->Contents();
			MoveCursor( m_buffer.size() );
			Redraw(true);
		} else if( c == KEY_DOWN ) {
			// next history
			if( m_history_iter == nullptr ) {
				return;
			}

			m_history_iter = m_history_iter->m_next;
			if( m_history_iter == nullptr ) {
				m_buffer = "";
			} else {
				m_buffer = m_history_iter->Contents();
			}
			MoveCursor( m_buffer.size() );
			Redraw(true);
		} else if( c == KEY_HOME ) {
			MoveCursor(0);
			Redraw();
		} else if( c == KEY_END ) {
			MoveCursor( m_buffer.size() );
			Redraw();
		}
	}
}

//void LineReader::PushHistory( const char *text ) {

//}

//-----------------------------------------------------------------------------
void LineReader::Process() {
	MoveCursor( m_cursor );
	int input = mvwgetch( m_window, 0, m_cursor-m_view );
	InputChar( input );
	System::Console::Print( "%d", input );
}

//-----------------------------------------------------------------------------
LineReader::HistoryEntry::HistoryEntry( const char *text ) {
	m_text = std::unique_ptr<char>( new char[strlen(text) + 1] );
	strcpy( m_text.get(), text );
}

//-----------------------------------------------------------------------------
LineReader::HistoryEntry::HistoryEntry( const std::string &text ) {
	HistoryEntry( text.c_str() );
}
 

//-----------------------------------------------------------------------------
}}