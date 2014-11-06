//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

namespace System { namespace ServerConsole {

class LineReader {

	enum {
		HISTORY_SIZE = 128,
		INPUT_LENGTH = 1024
	};

	char m_history[HISTORY_SIZE][INPUT_LENGTH]; // history of completed lines
	int  m_history_index; // navigation index when pressing up/down
	char m_buffer[1024]; // current line being written to
	int  m_cursor; // text caret position
	int  m_length; // length of current buffered line
	int  m_view; // position of leftmost char, the horizontal scrolling offset
	
public:

	/// -----------------------------------------------------------------------
	/// Construct the LineReader.
	///
	/// There should only be one instance of this.
	///
	/// @param window Window to edit while typing.
	///
	LineReader( WINDOW* window );

	/// -----------------------------------------------------------------------
	/// Input a character from the user
	///
	/// @param c Character to write.
	///
	void InputChar( char c );
};

}}