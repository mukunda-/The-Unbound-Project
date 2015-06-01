//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2015, Mukunda Johnson, All rights reserved. =========//

#pragma once

//-----------------------------------------------------------------------------
namespace Ui { namespace Error {

/** ---------------------------------------------------------------------------
 * List of error codes.
 */
enum {
	NAME_COLLISION,
	BAD_NAME
};

/** ---------------------------------------------------------------------------
 * Error base class.
 */
class Error : public std::runtime_error {

	int m_code;
	
protected:
	Error( int code, const Stref &text );

public:
	
	/** -----------------------------------------------------------------------
	 * Get the error code.
	 */
	int Code() { return m_code; }
	
	/** -----------------------------------------------------------------------
	 * Get the error message.
	 */
	std::string Message() { return what(); }
};

//-----------------------------------------------------------------------------
class NameCollision : public Error { public: NameCollision(); };
class BadName : public Error { public: BadName(); };

}}
