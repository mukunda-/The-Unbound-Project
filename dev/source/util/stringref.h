//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

#pragma once

namespace Util {

/// -----------------------------------------------------------------------
/// A class that simplifies string handling to lessen function overloads.
///
class StringRef {
	const char *m_text;

public:

	/// -----------------------------------------------------------------------
	/// Construct from string source. obtains const reference.
	///
	StringRef( const std::string &str ) : m_text( str.c_str() ) {}
	StringRef( const char *str        ) : m_text( str         ) {}
	
	/// -----------------------------------------------------------------------
	/// Get string.
	///
	const char * operator *() const {
		return m_text;
	}

	/// -----------------------------------------------------------------------
	/// Convert to const char *
	///
	operator const char*() const {
		return m_text;
	}
	
	/// -----------------------------------------------------------------------
	/// Copy to std::string
	///
	operator std::string() const {
		return m_text;
	}
};

}
