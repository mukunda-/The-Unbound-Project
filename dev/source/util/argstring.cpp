//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

#include <stdafx.h>
#include "util/argstring.h"

namespace Util {

//-----------------------------------------------------------------------------
namespace {

	template <typename F, typename ... Args>
	auto Convert( const char *ptr, bool easy, F converter, Args ... args ) ->
			decltype( converter( ptr, &v, args... )) {

		char *v;
		auto value = converter( ptr, &v, args... );
		if( easy ) return value;

		// "If the subject sequence is empty or does not have the expected 
		// form, no conversion shall be performed; the value of str is
		// stored in the object pointed to by endptr, provided that endptr
		// is not a null pointer."
		if( errno == ERANGE || errno == EINVAL || ptr == v ) {
			throw ArgString::BadInput();
		}

		return value;
	}
}

//-----------------------------------------------------------------------------
ArgString::ArgString( const Util::StringRef &input ) : m_source(input) {
	const char *source = *input;

	const char *term_start = *input;
	const char *term_end = *input;

	while( *source ) {
		
		// find next term
		while( std::isspace( *source ) ) {
			source++;
		}

		if( !*source ) break;

		if( *source == '"' ) {
			// quoted string
			source++;
			term_start = source;
			while( *source != 0 && *source != '"' ) {
				source++;
			}
			term_end = source;
			m_index.push_back( 
				Index::FromPointers( 
						*input, term_start, term_end ));
			source++;
		} else {
			// normal string
			term_start = source;
			while( *source != 0 && !std::isspace(*source) ) {
				source++;
			}
			term_end = source;
			m_index.push_back(
				Index::FromPointers(
						*input, term_start, term_end ));
			source++;
		}
	}
}

//-----------------------------------------------------------------------------
bool ArgString::Check( std::initializer_list<int> usage, 
					   int start, bool allow_extra ) {

	assert( start >= 0 );
	if( (start + (int)usage.size()) > m_index.size() ) {
		return false; // not enough arguments.
	}

	if( !allow_extra && (usage.size() + start) != m_index.size() ) {
		return false; // too many arguments
	}

	try {
	
		int pos = start;
		for( int type : usage ) {
			if( type == STRING ) {
				// expected argument, no conversion validation.

			} else if( type == INT ) {
				// these will throw an exception on failure
				GetInt( pos, false );
			} else if( type == LONG ) {
				GetLongInt( pos, false );
			} else if( type == FLOAT ) {
				GetFloat( pos, false );
			}
			pos++;
		}
		return true;

	} catch( BadInput & ) {}

	return false;
}

//-----------------------------------------------------------------------------
void ArgString::GetCString( int index, char *output, int maxlen ) const {
	assert( maxlen > 0 );
	assert( output );
	if( index < 0 || index >= m_index.size() ) {
		output[0] = 0;
		return;
	}

	int clipped_length = m_index[index].length;
	if( clipped_length > maxlen-1 ) clipped_length = maxlen-1;
	memcpy( output, (*m_source) + m_index[index].start, clipped_length );
	output[clipped_length] = 0;
}

//-----------------------------------------------------------------------------
int ArgString::GetInt( int index, bool easy, int base ) const { 
	if( index < 0 || index >= m_index.size() ) return 0;

	return Convert( (*m_source) + m_index[index].start, easy, 
					strtol, base );
}

//-----------------------------------------------------------------------------
long long ArgString::GetLongInt( int index, bool easy, int base ) const {
	if( index < 0 || index >= m_index.size() ) return 0;

	return Convert( (*m_source) + m_index[index].start, easy, 
					strtoll, base );
}

//-----------------------------------------------------------------------------
double ArgString::GetFloat( int index, bool easy ) const {
	if( index < 0 || index >= m_index.size() ) {
		return 0.0;
	}
		 
	return Convert( (*m_source) + m_index[index].start, easy, 
				strtod );
}

//-----------------------------------------------------------------------------
ArgString::BadInput::BadInput() : 
	std::runtime_error( "Argument conversion failed." )
{
}

//-----------------------------------------------------------------------------
}
