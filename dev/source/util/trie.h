//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//

// TRIE class
//
// associates strings with data
// words must only use ascii values 32-95 ie SPACE to UNDERSCORE
// 

// todo: optimize:
//   keep a counter for branches used and if the counter is "1"
//   a copy of the final branch can be used at that point

#pragma once

#include "mem/memorylib.h"

//-------------------------------------------------------------------------------------------------
namespace Util {



//-------------------------------------------------------------------------------------------------
template <class Type>
class Trie {

//-------------------------------------------------------------------------------------------------
private:

	static const int ASCII_BASE = 0;
	static const int BRANCH_ENTRIES = 128; 
	
	//-------------------------------------------------------------------------
	struct Branch : public Memory::FastAllocation {
		Type data;
		bool value_set;
		Branch *branches[BRANCH_ENTRIES]; 

		Branch() {
			value_set = false;
			memset( branches, 0, sizeof branches );
		}

		~Branch() {
			Clear();
		}
		
		void Clear() {
			for( uint32_t i = 0; i < BRANCH_ENTRIES; i++ ) {
				if( branches[i] ) {
					delete branches[i];
					branches[i] = nullptr;
				}
			}
		}
	};
	
	//-------------------------------------------------------------------------
	Branch trunk;

	
	//-------------------------------------------------------------------------
	Branch *FindKey( const char *key ) {
		Branch *b = &trunk;
		for( auto k = key; *k; k++ ) {
			int index = (*k) - ASCII_BASE;
			assert( index >= 0 && index < BRANCH_ENTRIES );
			if( b->branches[index] ) {
				b = b->branches[index];
			} else {
				return nullptr;
			}
		}
		if( !b->value_set ) return nullptr;
		return b;
	}
	  
//-------------------------------------------------------------------------------------------------
public:
	
	//-------------------------------------------------------------------------
	Trie() {
		memset( &trunk, 0, sizeof(Branch) );
	}
	
	//-------------------------------------------------------------------------
	void Clear() {
		trunk.Clear();
	}
	
	//-------------------------------------------------------------------------
	// set a key
	//
	//  key : name of key
	//  value : stored value
	//  replace : if key is already set, overwrite it
	//
	bool Set( const char *key, Type value, bool replace=true ) {
		Branch *b = &trunk;

		for( auto k = key; *k; k++ ) {
			int index = (*k) - ASCII_BASE;
			assert( index >= 0 && index < BRANCH_ENTRIES );
			if( !b->branches[index] ) {
				b->branches[index] = new Branch;
			}
			b = b->branches[index];
		}
		if( !replace && b->value_set ) return false;
		b->data = value;
		b->value_set = true;
		return true;
	}

	//-------------------------------------------------------------------------
	// reset a key
	//
	//  this checks if a key exists and then resets its 'set' flag and
	//  fills the data with the value specified (eg to reset a smart pointer)
	//
	// returns true if the key was reset, false if the key was not set
	//
	bool Reset( const char *key, Type value = 0 ) {
		Branch *b = FindKey(key);
		if( !b ) return false;
		b->data = value;
		b->value_set = false;
		return true;
	}
	
	//-------------------------------------------------------------------------
	bool Get( const char *key, Type &value ) {
		Branch *b = FindKey(key);
		if( !b ) return false;
		value = b->data;
		return true;
	}
};

}
