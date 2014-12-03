//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

#include <stdafx.h>

#pragma comment( lib, "gtest.lib" )
#pragma comment( lib, "libprotobuf.lib" )
#pragma comment( lib, "mysqlcppconn.lib" )
#pragma comment( lib, "pdcurses.lib" )
//-----------------------------------------------------------------------------

TEST( MiscTests, MyTest1 ) {
	ASSERT_EQ( 1, 1 ) << "My goodness!";
}

int main( int argc, char *argv[] ) {
	
	::testing::InitGoogleTest( &argc, argv );
	return RUN_ALL_TESTS();
}
