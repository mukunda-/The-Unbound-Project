//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//

#pragma once

#include "util/Trie.h"
#include "util/stringles.h"

//-------------------------------------------------------------------------------------------------
namespace Util {

class StringTable2 {
	Util::Trie<int> index_trie;
	std::vector<std::string> strings;
	bool case_sensitive;

public:
	StringTable2( bool p_case_sensitive = true ) {
		case_sensitive = p_case_sensitive;
	} 

	int Add( const char *text ) {
		int index = Find( text );
		if( index != -1 ) return index;
		strings.push_back( text );
		index_trie.Set( text, strings.size()-1 );
		return strings.size() - 1;
	}

	int Find( const char *text ) {
		int index;
		if( !index_trie.Get( text, index ) )
			return -1;
		return index;
	}
	
	void Erase() {
		strings.clear();
		index_trie.Clear();
	}

	void Read( int index, char *output, int maxlen ) const {
		Util::CopyString( output, maxlen, strings[index].c_str() );
	}

	template <size_t maxlen> void Read( int index, char (&output)[maxlen] ) const {
		Util::CopyString( output, strings[index].c_str() );
	}


};
}
