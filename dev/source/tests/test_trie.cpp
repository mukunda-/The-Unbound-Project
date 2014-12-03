//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

#include <stdafx.h>
#include "util/trie.h"
#include "util/stringles.h"

//-----------------------------------------------------------------------------
TEST( UtilTests, TrieTest ) {
	Util::Trie<int> test;
	
	EXPECT_TRUE( test.Set( "abc", 5 ) );

	int value;
	EXPECT_TRUE( test.Get( "abc", value ));
	EXPECT_EQ( value, 5 );
	
	
	EXPECT_TRUE( test.Set( "abc", 75, true ));
	EXPECT_TRUE( test.Get( "abc", value ));
	EXPECT_EQ( value, 75 );

	EXPECT_FALSE( test.Set( "abc", 12, false ));
	EXPECT_TRUE( test.Get( "abc", value ));
	EXPECT_EQ( value, 75 );

	test.Clear();
	EXPECT_FALSE( test.Get( "abc", value ));

	// test2..
	std::string base = Util::StringGarbage( 4 );
	EXPECT_EQ( base.size(), 4 );

	for( int i = 0; i < 20; i++ ) {
		std::string key = base + Util::StringGarbage( 128 );
		EXPECT_EQ( key.size(), 128+4 );
		
		value = rand();
		
		EXPECT_TRUE( test.Set( key, value ));

		int value2;
		EXPECT_TRUE( test.Get( key, value2 ));
		EXPECT_EQ( value, value2 );
	}

	test.Clear();
	for( int i = 0; i < 20; i++ ) {
		std::string key = base + Util::StringGarbage( 99 );
		EXPECT_EQ( key.size(), 99+4 );

		value = rand();
		EXPECT_TRUE( test.Set( key, value ));
		EXPECT_TRUE( test.Reset( key ));
		EXPECT_FALSE( test.Reset( key ));
	}
			
	test.Clear();
	
	for( int i = 0; i < 20; i++ ) {
		std::string key = base + Util::StringGarbage( 57 );
		EXPECT_EQ( key.size(), 4+57 );
		value = rand();
		EXPECT_TRUE( test.Set( key, value ));
		EXPECT_TRUE( test.Reset( key ));
		EXPECT_FALSE( test.Get( key, value ));
		EXPECT_FALSE( test.Reset( key ));
	}
			
	test.Clear();
	
	for( int i = 0; i < 20; i++ ) {
		std::string key = base + Util::StringGarbage( 16 );
		EXPECT_EQ( key.size(), 4+16 );
		value = rand();
		EXPECT_TRUE( test.Set( key, value ));
		value = rand();
		int value2;
		EXPECT_TRUE( test.Set( key, value ));
		EXPECT_FALSE( test.Set( key, value, false ));
		EXPECT_FALSE( test.Get( key.substr(0,4), value ));
		EXPECT_TRUE( test.Get( key, value2 ));
		EXPECT_EQ( value, value2 );
		EXPECT_TRUE( test.Reset( key ));
		EXPECT_FALSE( test.Reset( key ));
	}
} 
