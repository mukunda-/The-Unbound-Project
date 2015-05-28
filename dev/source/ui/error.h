//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2015, Mukunda Johnson, All rights reserved. =========//

#pragma once

//-----------------------------------------------------------------------------
namespace Ui { namespace Error {

//-----------------------------------------------------------------------------
enum {
	NAME_COLLISION,
	BAD_NAME
};

//-----------------------------------------------------------------------------
class Error : public std::runtime_error {

	int m_code;
	
protected:
	Error( int code, const Stref &text );

public:
	
	int Code() { return m_code; }
};

//-----------------------------------------------------------------------------
class NameCollision : public Error { public: NameCollision(); };
class BadName : public Error { public: BadName(); };

}}
