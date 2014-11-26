//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

#pragma once

namespace Util {

	/// -----------------------------------------------------------------------
	/// dumb std::atoi wrapper
	///
	int StringToInt( const char *str );

	/// -----------------------------------------------------------------------
	/// Convert a boolean string to a boolean value.
	///
	/// @param str String to check
	///
	/// @returns true if the string matches one of the true values, false if
	///          the string is a false value or invalid.
	///
	///          "true" values are "YES", "TRUE", or "ON"
	///          "false" values are "NO", "FALSE", or "OFF"
	///          not case sensitive
	///
	///          todo: 3 state return value to check for failure??
	///
	bool StringToBool( const char *str );

	/// -----------------------------------------------------------------------
	/// dumb std::atof wrapper
	///
	double StringToFloat( const char *str );

	/// -----------------------------------------------------------------------
	/// String crop (substring).
	///
	/// str = str[start..end]
	/// 
	/// @param str   String to crop.
	/// @param start Starting position of crop, 0 based.
	/// @param end   Ending position of crop, inclusive.
	void CropString( char *str, int start, int end );

	/// -----------------------------------------------------------------------
	/// str = str[start...]
	///
	/// @param str String to modify
	/// @param start 0 based offset from start of string to start crop.
	///
	void CropStringLeft( char *str, int start );

	/// -----------------------------------------------------------------------
	/// Remove all quote characters in a string.
	/// Yes...all of them.
	///
	/// @param str String to modify.
	///
	void StripQuotes( char *str );

	/// -----------------------------------------------------------------------
	/// Remove whitespace from beginning and end of string.
	///
	/// @param str String to modify.
	///
	void TrimString( char *str );

	/// -----------------------------------------------------------------------
	/// Extracts an argument from a string, cutting it out and copying it 
	/// to `dest`.
	///
	/// `source` will point to the next argument, or it will be empty.
	///
	void ScanArgString( char *source, char *dest, int maxlen );

	/// -----------------------------------------------------------------------
	/// Copy a string.
	///
	/// @param dest   Destination buffer.
	/// @param maxlen Size of destination buffer.
	/// @param source String to copy.
	///
	void CopyString( char *dest, size_t maxlen, const char *source );
	template <size_t maxlen>
	void CopyString( char (&dest)[maxlen], const char *source ) {
		return CopyString( dest, maxlen, source );
	}

	/// -----------------------------------------------------------------------
	/// Extract the first term from a string.
	///
	/// Copies the first term in `source` to `dest`, and returns a pointer 
	/// to the next arg in `source`.
	///
	/// Skips any whitespace before the term, and the pointer points to the
	/// next term after any whitespace too.
	///
	/// @param source String to scan.
	/// @param dest   Output buffer for the extracted term.
	/// @param maxlen Size of output buffer.
	/// @returns      Pointer to next term in `source`.
	///
	const char *BreakString( const char *source, char *dest, int maxlen );
	template <size_t maxlen> const char *BreakString( const char *source, char(&dest)[maxlen] ) {
		return BreakString( source, dest, maxlen );
	}

	/// -----------------------------------------------------------------------
	/// Test if a string is empty.
	///
	/// @param string Null terminated string.
	/// @returns true if the string is empty.
	///
	static inline bool StrEmpty( const char *string ) {
		return string[0] == 0;
	}

	/// -----------------------------------------------------------------------
	/// Test if two strings are equal.
	///
	/// @param a              First string to compare.
	/// @param b              Seconds string to compare.
	/// @param case_sensitive false to match regardless of letter case.
	///
	/// @returns true if the strings match.
	///
	bool StrEqual( const char *a, const char *b, bool case_sensitive = true );

	/// -----------------------------------------------------------------------
	/// Convert any letters in a string to uppercase.
	///
	/// @param str String to modify.
	///
	void StringToUpper( char *str );
	
	/// -----------------------------------------------------------------------
	/// Replace any characters above code 128.
	///
	/// @param str     String to modify.
	/// @param replace Character to replace with.
	///
	void StringASCIIFilter( char *str, char replace = '_' );

	/// -----------------------------------------------------------------------
	/// Replace any characters above code 128 or below 32.
	///
	/// @param str     String to modify.
	/// @param replace Character to replace with.
	///
	void StripASCIIControls( char *str, char replace = ' ' );
	
	/// -----------------------------------------------------------------------
	/// Generate a random string.
	///
	/// @param length Output length in characters.
	/// @returns String with random ascii characters in range [32,127]
	///
	std::string StringGarbage( int length );

	#ifdef EIGEN_CORE_H

	/// -----------------------------------------------------------------------
	/// Parse a color from a string.
	///
	/// @param input Input string to process
	///              Acceptable formats:
	///               "255 255 255 [255]" 8bit integers [alpha optional]
	///               "1.0 1.0 1.0 [1.0]" normalized decimal
	///               "999[9]"            quick format, 0-9 per component
	///               "ffffff[ff]"        hex code
	/// @param output Vector to store the parsed result 
	///               (in normalized format).
	/// @returns false if the input was invalid.
	///
	bool ParseColorString( const char *input, Eigen::Vector4f &output );
	#endif

	/// -----------------------------------------------------------------------
	/// Format a string with a decimal number.
	///
	/// @param input Number to format.
	/// @param digits Number of digits to keep after the decimal point.
	/// @returns Formatted string.
	///
	std::string RoundDecimal( double input, int digits = 0 );

	/// -----------------------------------------------------------------------
	/// Test if a string is all digits (0-9), and if it contains at least
	/// one character.
	///         
	bool IsDigits( const std::string &test );

	/// -----------------------------------------------------------------------
	/// Format a string.
	///
	/// @param format Template.
	/// @param args   Formatted arguments.
	/// @returns      Formatted string.
	///
	template< typename ... Args >
	std::string Format( const std::string &format, Args ... args ) {
		boost::format formatter(format);
		Util::Feed( formatter, args... );
		return formatter.str();
	}
}
