//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

#include "stdafx.h"
#include "util/stringtable.h"
#include "util/hashstring.h"
#include "util/stringles.h"

namespace Util {

//-------------------------------------------------------------------------------------------------
StringTable::StringTable( bool p_case_sensitive ) {
	case_sensitive = p_case_sensitive;
}

//-------------------------------------------------------------------------------------------------
StringTable::~StringTable() {
	////////////
}

//-------------------------------------------------------------------------------------------------
int StringTable::GetHash( const char *text ) const {
	return Util::HashString( text, case_sensitive );
	/*
	// todo: improved hashing
	int hash =0;
	for( int i = 0; text[i]; i++ ) {
		char c = text[i];
		if( !case_sensitive ) {
			c = tolower(c);
		}
		hash += (c+i) << (i & 15);
	}
	return hash;
	*/
}

//-------------------------------------------------------------------------------------------------
const StringTableEntry * StringTable::ReadEntry( uint32_t index ) const {
	if( index >= strings.size() ) return 0;
	return &strings[index];
}

//-------------------------------------------------------------------------------------------------
int StringTable::QueryIndex( int hash ) const {
	for( uint32_t i = 0; i < strings.size(); i++ ) {
		if( strings[i].hashcode == hash ) {
			return i;
		}
	}
	return -1;
}

//-------------------------------------------------------------------------------------------------
int StringTable::QueryIndex( const char *text, const void *data ) const {
	if( text != 0 ) {
		int hash = GetHash( text );
		return QueryIndex( hash );
	} else if( data != 0 ) {
		for( uint32_t i = 0; i < strings.size(); i++ ) {
			if( strings[i].data == data ) return i;
		}
		return -1;
	}
	return -1;
}

//-------------------------------------------------------------------------------------------------
void *StringTable::Query( int hash ) const {
	int index = QueryIndex( hash );
	if( index == -1 ) return 0;
	return strings[index].data;
}

//-------------------------------------------------------------------------------------------------
void *StringTable::Query( const char *text ) const {
	return Query( GetHash(text) );
}

//-------------------------------------------------------------------------------------------------
int StringTable::Add( const char *text, void *data ) {
	int hash = GetHash( text );
	int existing = QueryIndex( hash );
	if( existing != -1 ) return ADDSTRING_EXISTS;
	
	StringTableEntry e;
	e.data = data;
	e.hashcode = hash;
	e.text_length = strlen(text);
	e.text_offset = AddTextData( text );
	AddEntry( e );
	return ADDSTRING_COMPLETED;
}

//-------------------------------------------------------------------------------------------------
int StringTable::Remove( const char *text, void *data ) {
	int index = -1;
	if( text != 0 ) {
		// use text
		index = QueryIndex( text );
	} else if( data != 0 ) {
		index = QueryIndex( 0, data );
	} else {
		return REMOVESTRING_NOPARAMETERS;
	}

	if( index == -1 ) return REMOVESTRING_NOTFOUND;

	RemoveTextData( index );
	RemoveEntry( index );
	
	return REMOVESTRING_COMPLETED;
}

//-------------------------------------------------------------------------------------------------
int StringTable::AddTextData( const char *text ) {
	int offset = text_data.size();
	text_data += text;
	return offset;
}

//-------------------------------------------------------------------------------------------------
void StringTable::RemoveTextData( int index ) {
	StringTableEntry &e = strings[index];

	// todo: lookup how erase works
	text_data.erase( e.text_offset, e.text_length );
	for( uint32_t i = index+1; i < strings.size(); i++ ) {
		strings[i].text_offset -= e.text_length;
	}
	
	e.text_offset = 0;
	e.text_length = 0;
}

//-------------------------------------------------------------------------------------------------
void StringTable::AddEntry( StringTableEntry &entry ) {
	strings.push_back( entry );
}

//-------------------------------------------------------------------------------------------------
void StringTable::RemoveEntry( int index ) {
	for( uint32_t i = index+1; i < strings.size(); i++ ) {
		strings[i-1] = strings[i];
	}
	strings.pop_back();
}


}
