//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//

// TRIE class
// Associates strings with data 
// 

// todo: optimize:
//   keep a counter for branches used and if the counter is "1"
//   a copy of the final branch can be used at that point

#pragma once


#pragma warning( disable : 4351 )

#include "mem/memorylib.h"

//-----------------------------------------------------------------------------
namespace Util {



//-----------------------------------------------------------------------------
template <class Type>
class Trie {

//-----------------------------------------------------------------------------
private:

	static const int ASCII_BASE = 0;
	static const int BRANCH_ENTRIES = 128; 
	
	//-------------------------------------------------------------------------
	struct Branch : public Memory::FastAllocation {
		Type    m_data; 
		bool    m_isset = false; 
		Branch *m_branches[BRANCH_ENTRIES]; 

		//---------------------------------------------------------------------
		Branch() : m_branches() { 
		}

		//---------------------------------------------------------------------
		~Branch() {
			for( uint32_t i = 0; i < BRANCH_ENTRIES; i++ ) {
				if( m_branches[i] ) {
					delete m_branches[i];
					m_branches[i] = nullptr;
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
			if( b->m_branches[index] ) {
				b = b->m_branches[index];
			} else {
				return nullptr;
			}
		}
		if( !b->m_isset ) return nullptr;
		return b;
	}
	  
//-------------------------------------------------------------------------------------------------
public:
	
	//-------------------------------------------------------------------------
	Trie() {

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
			if( !b->m_branches[index] ) {
				b->m_branches[index] = new Branch;
			}
			b = b->m_branches[index];
		}
		if( !replace && b->m_isset ) return false;
		b->m_data = value;
		b->m_isset = true;
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
		b->m_data = value;
		b->m_isset = false;
		return true;
	}
	
	//-------------------------------------------------------------------------
	bool Get( const char *key, Type &value ) {
		Branch *b = FindKey(key);
		if( !b ) return false;
		value = b->m_data;
		return true;
	}
};

}
