//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

#pragma once

#include "util/stringles.h"
#include "util/stringref.h"

//-----------------------------------------------------------------------------
namespace Util {

/** ---------------------------------------------------------------------------
 * A class to parse a command string. This must be treated as a temporary
 * object and should not be stored in an object with a long lifetime.
 */
class ArgString {

	// the command string given
	std::string m_source;

	struct Index {
		int start;
		int length;

		static Index FromPointers( const char *source, 
								   const char *start, 
								   const char *end ) {
			
			Index idx;
			idx.start = (int)(start-source);
			idx.length = (int)(end-source) - idx.start;
			return idx;
		}
	};
	
	std::vector< Index > m_index;


public:

	/** -----------------------------------------------------------------------
	 *  Parse an argument string.
	 * 
	 *  @param input Line of text to parse into arguments.
	 */
	ArgString( const Util::StringRef &input );

	/** -----------------------------------------------------------------------
	 *  Checks that the arg string contains the desired input.
	 * 
	 *  @param usage       List of expected arguments.
	 *  @param start       Offset of argument list. Usually 1 to skip the 
	 *                     command name.
	 *  @param allow_extra If false, fail if there are more arguments present
	 *                     than desired. If true, ignore any additional
	 *                     arguments.
	 * 
	 *  @returns true if the arguments are present and can convert into the
	 *           desired types.
	 * 
	 *  For example Check( {STRING,STRING,INTEGER} ) will check if there are
	 *  at least 3 arguments (plus the command), and checks if the third 
	 *  command argument is a valid integer.
	 * 
	 *  STRING checks for any argument.
	 *  INT checks for a valid integer.
	 *  LONG checks for a valid 64-bit integer.
	 *  FLOAT checks for a valid decimal number.
	 */
	bool Check( std::initializer_list<int> usage, int start = 1, 
				bool allow_extra = true );
	
	enum {
		STRING,
		INT,
		LONG,
		FLOAT
	};

	/** -----------------------------------------------------------------------
	 *  @returns number of arguments parsed.
	 */
	int Count() const { return (int)m_index.size(); }

	// Note: All Get functions return an "empty" value if the
	// index is out of range.

	/** -----------------------------------------------------------------------
	 *  Get the full command string starting from the specified index.
	 * 
	 *  @param start Starting index to read from.
	 *  @returns Copy of argument.
	 * 
	 *  For example, with the input "aaa  bbb     cccc", 
	 *  GetFull(1) will return "bbb     cccc"
	 */
	std::string GetFull( int start ) const {
		if( start < 0 || start >= (int)m_index.size() ) return "";
		
		const Index &first = m_index[start];
		const Index &last = m_index[m_index.size()-1];

		return m_source
			.substr( first.start, last.start+last.length-first.start );
	}

	/** -----------------------------------------------------------------------
	 *  Read an argument as a string.
	 *  
	 *  @param index Argument index.
	 *  @returns Copy of argument.
	 */
	std::string GetString( int index ) const {
		if( index < 0 || index >= (int)m_index.size() ) return "";
		return m_source
			.substr( m_index[index].start, m_index[index].length );
	}
	
	/** -----------------------------------------------------------------------
	 *  Read an argument into the given buffer.
	 *  
	 *  @param index  Argument index.
	 *  @param output Buffer to copy to.
	 *  @param maxlen Size of buffer.
	 */
	void GetCString( int index, char *output, int maxlen ) const;

	/** -----------------------------------------------------------------------
	 *  Get an argument as pointers to the internal buffer.
	 * 
	 *  @param index  Index of argument.
	 *  @param start  (Output) String address.
	 *  @param length (Output) Length of string returned.
	 * 
	 *  If the index is out of range, *start will be set to nullptr and length
	 *  will be set to 0.
	 */
	void GetStrptr( int index, const char *&start, int &length ) {
		
		if( index < 0 || index >= (int)m_index.size() ) {
			start = nullptr;
			length = 0;
			return;
		}

		start = m_source.c_str() + m_index[index].start;
		length = m_index[index].length;
	}

	/** -----------------------------------------------------------------------
	 *  Read an integer value.
	 * 
	 *  @param index Index of argument.
	 *  @param easy  Retrun 0 if no conversion can be performed instead of
	 *               throwing an exception.
	 *  @param base  Base to use for conversion. If 0, the base will be
	 *               detected. (see std::stoi documentation)
	 * 
	 *  @throws BadConversion if the input cannot be converted to an integer.
	 */
	int GetInt( int index, bool easy = true, int base = 0 ) const;
	long long GetLongInt( int index, bool easy = true, int base = 0 ) const;
	
	/** -----------------------------------------------------------------------
	 *  Read a floating point value.
	 * 
	 *  @param index Index of argument.
	 *  @param easy  If true, 0.0 is returned on failure instead of throwing
	 *               an exception.
	 * 
	 *  @throws BadConversion if the input cannot be converted.
	 */
	double GetFloat( int index, bool easy = true ) const;

	/** -----------------------------------------------------------------------
	 *  Read an argument
	 * 
	 *  @param index Index of argument.
	 *  @returns argument, or "" if the index is out of bounds.
	 */
	std::string operator[]( int index ) const {
		return GetString( index );
	}

	//-------------------------------------------------------------------------
	class BadInput : public std::runtime_error {
	public:
		BadInput();
	};
};

}
