//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2015, Mukunda Johnson, All rights reserved. =========//

#include "stdafx.h"
#include "tostring.h"

namespace Util {

template<> std::string ToString( int value                ) { return std::to_string( value ); }
template<> std::string ToString( long value               ) { return std::to_string( value ); }
template<> std::string ToString( long long value          ) { return std::to_string( value ); }
template<> std::string ToString( unsigned value           ) { return std::to_string( value ); }
template<> std::string ToString( unsigned long value      ) { return std::to_string( value ); }
template<> std::string ToString( unsigned long long value ) { return std::to_string( value ); }
template<> std::string ToString( float value              ) { return std::to_string( value ); }
template<> std::string ToString( double value             ) { return std::to_string( value ); }
template<> std::string ToString( long double value        ) { return std::to_string( value ); }
template<> std::string ToString( const char *value        ) { return value; }
template<> std::string ToString( const Stref &value       ) { return value; }
template<> std::string ToString( const std::string &value ) { return value; }

}
