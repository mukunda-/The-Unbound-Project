//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2015, Mukunda Johnson, All rights reserved. =========//

#include "stdafx.h"
#include "tostring.h"
#include "format.h"

namespace Util {

std::string ToString( int value                ) { return std::to_string( value ); }
std::string ToString( long value               ) { return std::to_string( value ); }
std::string ToString( long long value          ) { return std::to_string( value ); }
std::string ToString( unsigned value           ) { return std::to_string( value ); }
std::string ToString( unsigned long value      ) { return std::to_string( value ); }
std::string ToString( unsigned long long value ) { return std::to_string( value ); }
std::string ToString( float value              ) { return std::to_string( value ); }
std::string ToString( double value             ) { return std::to_string( value ); }
std::string ToString( long double value        ) { return std::to_string( value ); }
std::string ToString( const char *value        ) { return value; }
std::string ToString( const Stref &value       ) { return value; }
std::string ToString( const std::string &value ) { return value; }

//-----------------------------------------------------------------------------
std::string ToString( const Eigen::Vector3f &v ) {
	return Util::Format( "< %.4f, %.4f, %.4f >", v[0], v[1], v[2] );
}
/*
template<> std::string ToString( Eigen::Vector3f &v ) { 
	return ToString< const Eigen::Vector3f& >( v );
}*/

}
