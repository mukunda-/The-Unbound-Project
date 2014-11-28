//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//


#include "util/minmax.h"
  
namespace Util {

/// ---------------------------------------------------------------------------
/// A line buffering utility. An interface to manage 
/// the history for a command line program.
///
class LineBuffer {
	 
	enum {
		DEFAULT_MEM_LIMIT = 4096,
		UNSELECTED = -1
	};

	struct Index {
		int position;
		int length;

		Index( int a_position, int a_length ) {
			position = a_position;
			length = a_length;
		}
	};

	std::string        m_data;
	std::vector<Index> m_index;
	int32_t            m_memory_limit;
	int32_t            m_iterator;
	
	/// -----------------------------------------------------------------------
	/// Read a string from the line buffer.
	///
	/// @param memory_limit_in_bytes Memory threshold that controls when
	///                              old lines are deleted.
	///
	std::string GetString( int index ) {
		return m_data.substr( m_index[index].position, 
							  m_index[index].length );
	}
	
	/// -----------------------------------------------------------------------
	/// Remove some lines from the beginning of the buffer
	///
	/// @param amount Number of lines to delete
	///
	void Prune( int amount ) {
		if( amount >= (int)m_index.size() ) {
			m_data = "";
			m_index.clear();
			return;
		}

		int total_length = 0;
		for( int i = 0; i < amount; i++ ) {
			total_length += m_index[i].length;
		}
		m_data.erase( 0, total_length );
		m_index.erase( m_index.begin(), m_index.begin()+amount );
		for( Index& a : m_index ) {
			a.position -= total_length;
		} 
		if( m_iterator != UNSELECTED ) {
			m_iterator = Util::Max( m_iterator - amount, 0 );
		}
	}

public:

	/// -----------------------------------------------------------------------
	/// Construct a line buffer.
	///
	/// @param memory_limit_in_bytes Memory threshold that controls when
	///                              old lines are deleted.
	///
	LineBuffer( int memory_limit_in_bytes = DEFAULT_MEM_LIMIT ) {
		m_memory_limit = memory_limit_in_bytes;
		m_iterator = UNSELECTED;
	}
	  
	/// -----------------------------------------------------------------------
	/// @returns false if the next Previous() will not have any data.
	///
	bool CheckPrevious() const {
		return !(m_iterator == 0 || m_index.size() == 0);
	}

	/// -----------------------------------------------------------------------
	/// @returns false if the next Next() will not have any data.
	///
	bool CheckNext() const {
		return !(m_iterator == UNSELECTED || m_iterator == (m_index.size()-1));
	}

	/// -----------------------------------------------------------------------
	/// @returns true if the internal iterator has something selected.
	///
	bool HasSelected() {
		return m_iterator != UNSELECTED;
	}
	
	/// -----------------------------------------------------------------------
	/// Get the previous line in the history.
	///
	/// @returns Line contents or an empty string if there is no more history.
	///
	std::string Previous() {
		if( m_index.size() == 0 ) return "";

		if( m_iterator == UNSELECTED ) {
			m_iterator = (int)m_index.size()-1;
		} else {
			m_iterator = Util::Max( m_iterator - 1, 0 );
		}

		return GetString( m_iterator ); 
	}
	
	/// -----------------------------------------------------------------------
	/// Get the next line in the history.
	///
	/// @returns Line contents or an empty string 
	///          if the iterator is at the front.
	///
	std::string Next() {
		if( m_iterator == UNSELECTED ) return "";
		m_iterator++;
		if( m_iterator >= (int)m_index.size() ) {
			m_iterator = UNSELECTED;
			return "";
		}

		return GetString( m_iterator ); 
	}

	/// -----------------------------------------------------------------------
	/// Get the current string that is selected.
	///
	/// @returns The current string that the internal iterator has selected
	///          or an emptry string if nothing is selected.
	///
	std::string Selected() {
		if( m_iterator == UNSELECTED ) return "";
		return GetString( m_iterator );
	}
	
	/// -----------------------------------------------------------------------
	/// Reset the iterator so that nothing is selected.
	///
	/// After this, Previous() will return the line in the very front of the
	/// history.
	///
	void ResetSelector() {
		m_iterator = UNSELECTED;
	}

	/// -----------------------------------------------------------------------
	/// Remove the selected history entry and reset the selector.
	///
	void DeleteSelected() {
		if( m_iterator == UNSELECTED ) return;
		
		int length = m_index[m_iterator].length;
		m_data.erase( m_index[m_iterator].position, 
					  length );
		m_index.erase( m_index.begin() + m_iterator );

		// compensate offsets
		for( int i = m_iterator; i < (int)m_index.size(); i++ ) {
			m_index[i].position -= length;
		}
		
		ResetSelector();
	}
	
	/// -----------------------------------------------------------------------
	/// Push a line to the end of the history buffer.
	///
	/// @param text Line contents.
	///
	void Push( const std::string &text ) {
		int position = (int)m_data.size();
		m_data += text;
		m_index.push_back( Index( position, (int)text.size() ) );

		if( (int)m_data.size() > m_memory_limit ) {
			Prune( 10 );
		}
	}

	void Push( const char *text ) {
		Push( std::string( text ) );
	}

};

}