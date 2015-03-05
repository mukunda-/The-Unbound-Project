//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2015, Mukunda Johnson, All rights reserved. =========//

#pragma once

namespace Util {

/** ---------------------------------------------------------------------------
 * A string reference.
 *
 * This is a lightweight class that simplifies accepting a string argument
 * for functions. It accepts either C style const char* or C++ style
 * std::string and then provides access to either.
 */
class StringRef {
	const char *m_text; // pointer to the data

	// pointer to a std::string, if constructed with one
	// this will also be created if it doesn't exist and Str() is used.
	const std::string *m_str = nullptr;
	
	// for when Str() is used without a std::string source, the data
	// will be copied here, and m_str will point here.
	std::string m_localstr;

public:
	
	StringRef() {
		m_text = "";
	}

	/** -----------------------------------------------------------------------
	 * Construct from a string source. Obtains a const reference.
	 */
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

	/** -----------------------------------------------------------------------
	 * Methods to convert to const char*
	 */

	// named function
	const char *CStr() const {
		return m_text;
	}

	// dereference
	const char * operator *() const {
		return m_text;
	}
	
	/** -----------------------------------------------------------------------
	 * Copy to a new std::string
	 */

	// assignment
	operator std::string() const {
		return std::string( m_text );
	}
	
	// named function
	std::string Copy() const {
		return m_text;
	}

	/** -----------------------------------------------------------------------
	 * Convert to a const std::string.
	 *
	 * If this was instantiated using another std::string, that one is
	 * returned, otherwise, if created using a C string, this will create
	 * a temporary std::string to be used.
	 */
	const std::string &Str() const {
		if( m_str ) {
			return *m_str;
		} else {
			// a little bit of const magic, technically this is still
			// a const function because the visible state isn't changed
			const_cast<StringRef*>(this)->m_localstr = m_text;
			return m_localstr;
		}
	}

	/** -----------------------------------------------------------------------
	 * Returns true if the string is empty.
	 */
	bool Empty() {
		return m_text[0] == 0;
	}

	/** -----------------------------------------------------------------------
	 * Stream overload.
	 */
	friend std::ostream& operator<<(std::ostream &os, const StringRef &str ) {
		os << str.CStr();
		return os;
	}
};

}

/** ---------------------------------------------------------------------------
 * Since this is used so often, we will define it globally as a "Stref"
 */
using Stref = Util::StringRef;
