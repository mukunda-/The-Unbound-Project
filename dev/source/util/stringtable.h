//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//

//-------------------------------------------------------------------------------------------------
// class which associates strings with data pointers
//

// todo: multithreaded

#pragma once
 

#include <string>
#include <vector>

namespace Util {

//-------------------------------------------------------------------------------------------------
typedef struct t_StringTableEntry {
	int text_offset;	// offset into text_data
	short text_length;	// length of string
	int hashcode;		// hash code of string
	void *data;			// data pointer
} StringTableEntry;

//-------------------------------------------------------------------------------------------------
class StringTable {
	
	//--------------------------------------------------------
	std::string text_data;
	std::vector<StringTableEntry> strings;

	bool case_sensitive;
	
	//--------------------------------------------------------
	void AddEntry( StringTableEntry &entry );
	void RemoveEntry( int index );
	//--------------------------------------------------------
	int AddTextData( const char *text );
	void RemoveTextData( int index );

public:

	//--------------------------------------------------------
	StringTable( bool p_case_sensitive = true );
	~StringTable();

	//--------------------------------------------------------
	enum {
		ADDSTRING_EXISTS = -1,
		ADDSTRING_COMPLETED = 0
	};

	//--------------------------------------------------------
	enum {
		REMOVESTRING_NOTFOUND = -1,
		REMOVESTRING_NOPARAMETERS = -2,
		REMOVESTRING_COMPLETED = 0
	};

	//--------------------------------------------------------
	// add an entry to the string table
	//
	// text: name of entry (unique identifier)
	// data: pointer associated with string
	//
	int Add( const char *text, void *data );

	//--------------------------------------------------------
	// remove an entry from the string table
	// 
	// if text is specified, it will remove the single entry
	// with the matching name
	//
	// if text is 0 and data is specified, it will remove
	// all entries with matching data pointers
	//
	// if both parameters are 0 then it will do nothing...
	//
	int Remove( const char *text, void *data = 0 );

	//--------------------------------------------------------
	// search the string table for a match and get the
	// data pointer
	// 
	// text: string to compare against
	//
	void * Query( const char *text ) const;
	void * Query( int hash ) const;
	
	//--------------------------------------------------------
	// search the string table for a match and get the
	// index of the entry
	//
	// for text/data, specify only one, leave the other 0
	// for data lookups it will return the first entry
	//
	int QueryIndex( const char *text, const void *data = 0 ) const;
	int QueryIndex( int hash ) const;

	//--------------------------------------------------------
	// read data of an entry
	//
	const StringTableEntry *ReadEntry( int index ) const;

	//--------------------------------------------------------
	// create hash code for a string
	//
	int GetHash( const char *text ) const;
};

//-------------------------------------------------------------------------------------------------
}
