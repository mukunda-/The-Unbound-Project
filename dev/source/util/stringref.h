//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

#pragma once

namespace Util {

/// ---------------------------------------------------------------------------
/// A class that simplifies string handling to lessen function overloads.
///
class StringRef {
	const char *m_text;
	const std::string *m_str = nullptr;

	std::string m_localstr;

public:
	
	StringRef() {
		m_text = "";
	}

	/// -----------------------------------------------------------------------
	/// Construct from string source. obtains const reference.
	///
	StringRef( const std::string &str ) : 
		m_text( str.c_str() ), m_str( &str ) {}

	StringRef( const char *str ) : 
		m_text( str ) {}

	StringRef& operator=( const char *str ) {
		m_text = str;
		m_str  = nullptr;
	}

	StringRef& operator=( const std::string &str ) {
		m_text = str.c_str();
		m_str  = &str;
	}

	// ------------------------------------------------------------------------
	// Convert to const char *
	// ------------------------------------------------------------------------

	// named function
	const char *CStr() const {
		return m_text;
	}

	// dereference
	const char * operator *() const {
		return m_text;
	}
	
	// ------------------------------------------------------------------------
	// Copy to std::string
	// ------------------------------------------------------------------------
	
	// assignment
	operator std::string() const {
		return std::string(m_text);
	}
	
	// named function
	std::string Copy() const {
		return m_text;
	}

	const std::string &Str() const {
		if( m_str ) {
			return *m_str;
		} else {
			const_cast<StringRef*>(this)->m_localstr = m_text;
			return m_localstr;
		}
	}

	// ------------------------------------------------------------------------
	// Some utility functions.
	// ------------------------------------------------------------------------

	bool Empty() {
		return m_text[0] == 0;
	}
};

}
