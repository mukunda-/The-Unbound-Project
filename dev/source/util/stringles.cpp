//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//

#include "stdafx.h"
#include "util/minmax.h"
#include "util/argstring.h"
  
namespace Util {
	
//-------------------------------------------------------------------------------------------------
int StringToInt( const char *str ) {
	return std::atoi( str );
}

//-------------------------------------------------------------------------------------------------
double StringToFloat( const char *str ) {
	return std::atof( str );
}

//-------------------------------------------------------------------------------------------------
bool StringToBool( const char *str ) {
	if( StrEqual(str,"yes",false) ) return true;
	//if( StrEqual(str,"no",false) ) return false;

	if( StrEqual(str,"true",false) ) return true;
	//if( StrEqual(str,"false",false) ) return false;

	if( StrEqual(str,"on",false) ) return true;
	//if( StrEqual(str,"off",false) ) return false;

	return false;
}

//-------------------------------------------------------------------------------------------------
void CropString( char *str, int start, int end ) {
	// copy string
	int i;
	for( i = start; i <= end; i++ ) {
		str[i-start] = str[i];
	}
	str[i-start] = 0;
}

//-------------------------------------------------------------------------------------------------
void CropStringLeft( char *str, int start ) {
	int i;
	for( i = start; str[i]; i++ ) {
		str[i-start] = str[i];
	}
	str[i-start] = 0;
}

//-------------------------------------------------------------------------------------------------
void StripQuotes( char *str ) {
	// todo; better function

	int a = strlen(str);
	if( a <= 1 ) return;
	a--;
	if( str[a] == '"' && str[0] == '"' ) {
		CropString( str, 1, a-1 );
	}
	/*
	int write = 0;
	for( int i = 0; str[i]; i++ ) {
		if( str[i] != '"' ) {
			str[write++] = str[i];
		}
	}
	str[write] = 0;
	*/

}

//-------------------------------------------------------------------------------------------------
void TrimString( char *str ) {
	int start, end, len;
	int i;

	// find start
	for( i = 0; str[i]; i++ ) {
		if( str[i] < 1 || str[i] > 32 ) break;
	}

	start = i;
	if( str[start] == 0 ) {
		// string is empty
		str[0] = 0; 
		return;
	}

	// find total length
	for( i = start; str[i]; i++ ) {
	}
	len = i;

	// find end
	for( i = len-1; ( str[i] >= 1 && str[i] <= 32 ); i-- ) {
	}
	end = i;

	CropString( str, start, end );
	
	
}

//-------------------------------------------------------------------------------------------------
bool DelimScan( char source, const char *delimiters ) {
	for( int i = 0; delimiters[i]; i++ ) {
		if( source == delimiters[i] ) return true;
	}
	return false;
}

//-------------------------------------------------------------------------------------------------
bool IsWhitespace( char a ) {
	return a == ' ' || a == '\t';
}

//-------------------------------------------------------------------------------------------------
void ScanArgString( char *source, char *dest, int maxlen ) {
	// takes first arg found in source and moves to dest
	// example source= "aaaaaa bbbb ccc":
	//     output source = "bbbb ccc"
	//     output dest   = "aaaaaa"
	TrimString(source);

	int len = 0;

	bool quotemode = false;

	int write = 0;
	int i;
	for( i = 0; source[i]; ) {
		
		if( quotemode ) {
			if( source[i] == '"' ) {
				i++;
				quotemode = false;
			} else {
				dest[write++] = source[i++];
				if( write == maxlen-1 ) break;
			}
		} else {
			if( source[i] == '"' ) {
				i++;
				quotemode = true;
			} else {
				if( IsWhitespace(source[i]) ) {
					i++;
					break;
				} else {

					dest[write++] = source[i++];
					if( write == maxlen-1 ) break;
				}
			}
		}
	}
	dest[write] = 0;

	if( source[i] ) { 
		CropStringLeft( source, i );
	} else {
		source[0] = 0;
	}

	TrimString( source );
	TrimString( dest );
	StripQuotes( dest );
}

//-------------------------------------------------------------------------------------------------
const char *BreakString( const char *source, char *dest, int maxlen ) {
	// copies the first arg in source to dest
	// and returns a pointer to the next arg in source
	//
	const char *read = source;
	bool quotes=false;
	int space = maxlen-1;
	assert( space != 0 );

	// left-trim
	// " <= ' ' " meaning skip space and all control characters
	while( (*read) <= ' ' && (*read) != 0 ) { 
		read++;
	}

	if( (*read) == '"' ) {
		*read++;

		// copy until end quote is found
		while( (*read) != 0 && (*read) != '"' ) {
			*dest++ = *read++;
			space--;
			if( space == 0 ) { *dest=0; return read; }
			
		}
		*dest = 0; //null termination
 		if( *read == 0 ) return read;
		read++; // skip quote
		
	} else {
		while( (*read) != 0 && (*read) > ' ' ) {
			*dest++ = *read++;
			space--;
			if( space == 0 ) { *dest=0; return read; }
			
		}
		*dest = 0;
	}

	// search for next arg
	while( (*read) <= ' ' && (*read) != 0 ) { 
		read++;
	}
	return read;
}

//-------------------------------------------------------------------------------------------------
void CopyString( char *dest, size_t maxlen, const char *source ) {
#if _MSC_VER

	strcpy_s( dest, maxlen, source );
#else

	if( strlen(source) > maxlen-1 ) {
		strncpy( dest, source, maxlen-1 );
		dest[maxlen-1] = 0;
//		return false;
	} else {
		strcpy( dest, source );
//		return true;
	}
#endif
}

//-------------------------------------------------------------------------------------------------
bool StrEqual( const char *a, const char *b, bool case_sensitive ) {
	if( case_sensitive == false ) {
		return strcmp( a,b ) == 0;
	} else {
		//todo: optimize (use a table at least)
		for( int i = 0;; i++ ) {
			char c1, c2;
			c1 = a[i];
			c2 = b[i];
			if( c1 >= 'A' && c1 <= 'Z' ) c1 += 'a' - 'A';
			if( c2 >= 'A' && c2 <= 'Z' ) c2 += 'a' - 'A';
			if( c1 != c2 ) return false;
			if( c1 == 0 ) return true;
		}
	}
}

//-------------------------------------------------------------------------------------------------
void StringToUpper( char *str ) {
	for( ; *str; str++ ) {
		if( *str >= 'a' && *str <= 'z' ) {
			*str += 'A' - 'a';
		}
	}
}

//-------------------------------------------------------------------------------------------------
void StringASCIIFilter( char *str, char replace ) {
	for( ; *str; str++ ) {
		if( *str >= 128 ) *str = replace;
	}
}

//-------------------------------------------------------------------------------------------------
void StripASCIIControls( char *str, char replace ) {
	for( ; *str; str++ ) {
		if( *str >= 128 || *str < 32 ) *str = replace;
	}
}

//-------------------------------------------------------------------------------------------------
void StripASCIIControls( std::string &str, char replace ) {
	 
	for( std::string::iterator a = str.begin(); a != std::string::iterator(); a++ ) {
		if( *a >= 128 || *a < 32 ) *a = replace;
	}
}

int ConvertHexDigit( char c ) {
	c = tolower(c);
	if( c >= '0' && c <= '9' ) {
		return c - '0';
	} else if( c >= 'a' && c <= 'f' ) {
		return c - 'a';
	} else {
		return 0;
	}
}

int ConvertHexByte( const char *str ) {
	return ConvertHexDigit( str[0] ) * 16 + ConvertHexDigit( str[1] );
}

#ifdef EIGEN_CORE_H
//-------------------------------------------------------------------------------------------------
bool ParseColorString( const char *input, Eigen::Vector4f &output  ) {
	// parses a color string into a vector
	
	//

	output.setOnes();

	ArgString args(input);
	if( args.Count() == 0 || args.Count() > 3 ) {
		return false;
	}

	if( args.Count() == 3 || args.Count() == 4 ) {
		if( args[0].find_first_of( '.' ) != std::string::npos ) {
			// r g b decimal
			for( int i = 0; i < args.Count(); i++ )
				output[i] = (float)Util::Clamp( std::atof( args[i].c_str() ), 0.0,1.0 );
			
		} else {
			// rrr ggg bbb
			for( int i = 0; i < args.Count(); i++ )
				output[i] = (float)Util::Clamp( (float)std::atoi( args[i].c_str() ) / 255.0, 0.0, 1.0 );
			
		}
	} else if( args.Count() == 1 ) {
		const std::string &arg = args[0];
		if( arg.size() == 3 || arg.size() == 4 ) {
			for( uint32_t i = 0; i < arg.size(); i++ )
				output[i] = Util::Clamp( (float)(arg[i]-'0') / 9.0f, 0.0f, 0.1f );
			
		} else if( arg.size() == 6 || arg.size() == 8 ) {
			const char *cstr = arg.c_str();
			 
			for( uint32_t i = 0; i < arg.size()/2; i++ ) {
				output[i] = Util::Clamp( (float)ConvertHexByte( &cstr[i*2] ) / 255.0f, 0.0f, 0.1f );
			}
			
		} else {
			return false;
		}
	}
	return true;
}
#endif

//-------------------------------------------------------------------------------------------------
std::string RoundDecimal( double input, int digits  ) {
	double scale = pow(10,digits);
	input = floor( input * scale + 0.5 );
	input = input / scale;
	return std::to_string(input);
}

//-------------------------------------------------------------------------------------------------
std::string StringGarbage( int length ) {
	std::string str;
	for( int i = 0; i < length; i++ ) {
		double d = (double)rand();
		d = d * 94.0;
		d = d / (double)RAND_MAX;
		d = floor( d + 0.5 );
		str += (char)(' ' + (int)d);
	}
	return str;
}

//-------------------------------------------------------------------------------------------------
bool IsDigits( const std::string &test ) {
	if( test.size() == 0 ) return false;

	for( char a: test ) {
		if( a < '0' || a > '9' ) return false;
	}

	return true;
}

}
