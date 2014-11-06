//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

namespace System { namespace ServerConsole {

LineReader::LineReader() {
	m_cursor = 0;
	m_length = 0;
	m_history_index = 0;

}

void LineReader::MoveCursor( int pos ) {
	g_cursor = pos;
	if( 
}

void LineReader::InputChar( char c ) {
	
	if( c >= 32 && c < 127 ) {
		m_buffer[g_cursor] = c;
		if( m_cursor < m_input_length ) {
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

}}