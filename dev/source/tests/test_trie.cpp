//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

#include <stdafx.h>
#include "trie.h"
#include "util/trie.h"
#include "util/stringles.h"
#include "system/console.h"

//-----------------------------------------------------------------------------
namespace Tests {
	Trie::Trie() {}

	//-------------------------------------------------------------------------
	bool Trie::Execute() {
		using System::Console::Print;
		
		{
			Print( "test1..." );
			Util::Trie<int> test;
			test.Set( "abc", 5 );
			int value;
			if( !test.Get( "abc",value ) ) return false;
			if( value != 5 ) return false;

			test.Set( "abc", 75, true );
			if( !test.Get( "abc", value ) ) return false;
			if( value != 75 ) return false;

			test.Set( "abc", 12, false );
			if( !test.Get( "abc", value ) ) return false;
			if( value != 75 ) return false;

			test.Clear();
			if( test.Get( "abc", value ) ) return false;

			Print( "test2..." );
			std::string base = Util::StringGarbage( 4 );

			for( int i = 0; i < 20; i++ ) {
				std::string key = base + Util::StringGarbage( 16 );
				value = rand();
				test.Set( key, value );
				int value2;
				if( !test.Get( key, value2 ) ) return false;
				if( value2 != value ) return false;
			}
			test.Clear();
			Print( "test3..." );
			for( int i = 0; i < 20; i++ ) {
				std::string key = base + Util::StringGarbage( 16 );
				value = rand();
				test.Set( key, value );
				if( !test.Reset( key ) ) return false;
				if( test.Reset( key ) ) return false;
			}
			
			test.Clear();
			Print( "test4..." );
			for( int i = 0; i < 20; i++ ) {
				std::string key = base + Util::StringGarbage( 16 );
				value = rand();
				test.Set( key, value );
				if( !test.Reset( key ) ) return false;
				if( test.Get( key, value ) ) return false;
				if( test.Reset( key ) ) return false;
			}
			
			test.Clear();
			Print( "test5..." );
			for( int i = 0; i < 20; i++ ) {
				std::string key = base + Util::StringGarbage( 16 );
				value = rand();
				test.Set( key, value );
				value = rand();
				int value2;
				if( !test.Set( key, value ) ) return false;
				if( test.Set( key, value, false ) ) return false;
				if( test.Get( key.substr(0,4), value ) ) return false;
				if( !test.Get( key, value2 ) ) return false;
				if( value2 != value ) return false;
				if( !test.Reset( key ) ) return false;
				if( test.Reset( key ) ) return false;
			}
			
		}
		return true;
	}
}