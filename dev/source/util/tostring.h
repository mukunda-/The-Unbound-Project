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
std::string ToString( int                );
std::string ToString( long               );
std::string ToString( long long          );
std::string ToString( unsigned           );
std::string ToString( unsigned long      );
std::string ToString( unsigned long long );
std::string ToString( float              );
std::string ToString( double             );
std::string ToString( long double        );

// string types
std::string ToString( const char *        );
std::string ToString( const Stref &       );
std::string ToString( const std::string & );

// eigen types
std::string ToString( const Eigen::Vector3f& );
//template<> std::string ToString<float&>( float & );
//template<> std::string ToString( Eigen::Vector3f & );
 
}