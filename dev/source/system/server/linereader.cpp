//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

#include "stdafx.h"
#include "system/server/serverconsole.h"
#include "linereader.h"
#include "system/system.h"
#include "system/console.h"

namespace System { namespace ServerConsole {

//-----------------------------------------------------------------------------
LineReader::LineReader( WINDOW  *window ) {
	m_cursor = 0; 
	m_window = nullptr;
	m_view = 0; 

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
static void ExecuteCommand( std::string buffer ) {
	System::Console::Execute( buffer.c_str() );
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
		if( c == '\n' || c == '\r' ) {
			// submit input.

			if( m_history.HasSelected() ) {
				if( m_history.Selected() == m_buffer ) {
					m_history.DeleteSelected();
				}
				m_history.ResetSelector();
			}

			if( m_buffer == "" ) return;
			m_history.Push( m_buffer );
			
			// execute command.
			// m_buffer
			System::Post( std::bind( ExecuteCommand, m_buffer ) );
			//System::Console::Execute( m_buffer.c_str() );

			m_buffer = "";
			MoveCursor( 0 );
			Redraw(true);

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
			if( !m_history.CheckPrevious() ) return;

			if( !m_history.HasSelected() ) {
				// if a history item isn't selected and the user
				// has typed something new, then add it to the
				// history before switching.
				if( m_buffer != "" ) {
					m_history.Push( m_buffer );
					m_history.Previous(); // skip what we just pushed
				} 
			}

			m_buffer = m_history.Previous(); 

			MoveCursor( m_buffer.size() );
			Redraw(true);
		} else if( c == KEY_DOWN ) {
			// next history
			   
			if( m_history.HasSelected() ) {
				m_buffer = m_history.Next();
			} else {
				if( m_buffer != "" ) {
					m_history.Push( m_buffer );
				}
				m_buffer = "";
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
	wmove( m_window, 0, m_cursor - m_view );
	Update();
}
 
//-----------------------------------------------------------------------------
bool LineReader::Process( int key ) {

	MoveCursor( m_cursor );
	//int input = mvwgetch( m_window, 0, m_cursor-m_view );
 
	InputChar( key ); 
	return true;
}
  
//-----------------------------------------------------------------------------
}}