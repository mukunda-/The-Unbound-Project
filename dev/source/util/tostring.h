//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2015, Mukunda Johnson, All rights reserved. =========//

// function to convert various types to a string

namespace Util {

// Default:
template< typename T > std::string ToString( T value ) { 
	return "<unknown type>"; 
}

// std::to_string wrappers
template<> std::string ToString( int value                );
template<> std::string ToString( long value               );
template<> std::string ToString( long long value          );
template<> std::string ToString( unsigned value           );
template<> std::string ToString( unsigned long value      );
template<> std::string ToString( unsigned long long value );
template<> std::string ToString( float value              );
template<> std::string ToString( double value             );
template<> std::string ToString( long double value        );

// string types
template<> std::string ToString( const char *value        );
template<> std::string ToString( const Stref &value       );
template<> std::string ToString( const std::string &value );
 
}