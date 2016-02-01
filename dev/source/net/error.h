//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2016, Mukunda Johnson, All rights reserved. =========//

#pragma once

namespace Net {

	class Error : public std::exception {};
	class WriteError : public Error {};
	class ParseError : public Error {};

}
