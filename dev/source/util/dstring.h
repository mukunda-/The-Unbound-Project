//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//

#if 0
// dynamic string utility class
//

#pragma once

#ifndef DString_H
#define DString_H

//-------------------------------------------------------------------------------------------------
class DString {
 
private:
	char *data;
	int length;

	void CacheLength();
	
public:
	DString();
	DString(const char *);
	~DString();

	const char *CString() const;

	operator =(DString&);
	operator +=(char);

	int Length();
	bool Empty();
};

#endif

#endif
