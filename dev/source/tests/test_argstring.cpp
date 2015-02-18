//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

#include <stdafx.h>
#include "util/argstring.h"

namespace Tests {

///////////////////////////////////////////////////////////////////////////////
TEST( UtilTests, ArgStringTest ) {
	using AS = Util::ArgString;
	AS as( " test1 \"test 2\" 5  " );

	//////////////////////////////////////////////////////////////////////
	EXPECT_TRUE( as.Check( { AS::STRING, AS::STRING, AS::INT }, 0 ));
	EXPECT_FALSE( as.Check( { AS::STRING, AS::STRING, AS::INT, AS::INT }, 0 ));
	EXPECT_TRUE( as.Check( { AS::STRING, AS::STRING }, 0 ));
	EXPECT_TRUE( as.Check( { AS::STRING, AS::INT }, 1, false ));
	EXPECT_TRUE( as.Check( { AS::INT }, 2, false ));
	EXPECT_FALSE( as.Check( { AS::INT } ));
	EXPECT_FALSE( as.Check( { AS::FLOAT } ));
	EXPECT_TRUE( as.Check( { AS::STRING } ));
	EXPECT_FALSE( as.Check( { AS::STRING }, 0, false ));

	EXPECT_EQ( "test1 \"test 2\" 5", as.GetFull( 0 ) );
	EXPECT_EQ( "test 2\" 5", as.GetFull( 1 ) );

	//////////////////////////////////////////////////////////////////////
	EXPECT_EQ( 0, as.GetInt(0) );
	EXPECT_EQ( 0, as.GetInt(1) );
	EXPECT_EQ( 5, as.GetInt(2) );

	//////////////////////////////////////////////////////////////////////
	EXPECT_EQ( "test1", as.GetString(0) );
	EXPECT_EQ( "test 2", as.GetString(1) );
	EXPECT_EQ( "5", as.GetString(2) );
	
	//////////////////////////////////////////////////////////////////////
	EXPECT_EQ( 0.0, as.GetFloat(0) );
	EXPECT_EQ( 0.0, as.GetFloat(1) );
	EXPECT_EQ( 5.0, as.GetFloat(2) );

	//////////////////////////////////////////////////////////////////////
	char cstring[512];
	cstring[7] = 'Y';

	as.GetCString( 0, cstring, sizeof cstring );
	EXPECT_STREQ( "test1", cstring );
	EXPECT_EQ( 'Y', cstring[7] );

	as.GetCString( 1, cstring, sizeof cstring );
	EXPECT_STREQ( "test 2", cstring );
	EXPECT_EQ( 'Y', cstring[7] );

	as.GetCString( 2, cstring, sizeof cstring );
	EXPECT_STREQ( "5", cstring );
	EXPECT_EQ( 's', cstring[2] ); // "5_st 2_Y..."
	EXPECT_EQ( 'Y', cstring[7] );

	//////////////////////////////////////////////////////////////////////
	const char *strptr;
	int length;
	as.GetStrptr( 0, strptr, length );
	EXPECT_EQ( 5, length );
	EXPECT_EQ( 't', strptr[0] );
	EXPECT_EQ( 'e', strptr[1] );
	EXPECT_EQ( '1', strptr[4] );

	as.GetStrptr( 1, strptr, length );
	EXPECT_EQ( 6, length );
	EXPECT_EQ( 't', strptr[0] );
	EXPECT_EQ( 'e', strptr[1] );
	EXPECT_EQ( '2', strptr[5] );

	as.GetStrptr( 2, strptr, length );
	EXPECT_EQ( 1, length );
	EXPECT_EQ( '5', strptr[0] );
	
}

///////////////////////////////////////////////////////////////////////////////
TEST( UtilTests, ArgStringTest2 ) {
	// no closing quote behavior

	using AS = Util::ArgString;
	AS as( " test1 \"test 2 551" );

	EXPECT_EQ( "test1", as.GetString(0) );
	EXPECT_EQ( "test 2 551", as.GetString(1) );
	EXPECT_EQ( "test1 \"test 2 551", as.GetFull(0) );
	EXPECT_EQ( "test 2 551", as.GetFull(1) );
}

///////////////////////////////////////////////////////////////////////////////
TEST( UtilTests, ArgStringTest3 ) {
	// numbers

	using AS = Util::ArgString;
	AS as( "   999.0 999999999999 999999999999999999999999999999999 2.5 1.875 " );
	
	EXPECT_EQ( 999, as.GetInt(0) );
	EXPECT_EQ( 999.0, as.GetFloat(0) );
	EXPECT_EQ( 2147483647, as.GetInt(1) );
	EXPECT_EQ( 999999999999L, as.GetLongInt(1) );
	EXPECT_EQ( 999999999999.0, as.GetFloat(1) );
	EXPECT_EQ( 2147483647, as.GetInt(2) );
	EXPECT_EQ( 9223372036854775807L, as.GetLongInt(2) );
	EXPECT_EQ( 2.5, as.GetFloat(3) );
	EXPECT_EQ( 2, as.GetInt(3) );
	EXPECT_EQ( 1.875, as.GetFloat(4) );
	EXPECT_EQ( 1, as.GetInt(4) );
}

///////////////////////////////////////////////////////////////////////////////
TEST( UtilTests, ArgStringTest4 ) {
	// single term

	using AS = Util::ArgString;
	AS as( std::string("quit") );
	
	EXPECT_EQ( "quit", as.GetString(0) );
}

}
