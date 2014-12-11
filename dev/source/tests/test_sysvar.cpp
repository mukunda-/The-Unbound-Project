//===========================  The Unbound Project  =========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

#include <stdafx.h>
#include "system/variables.h"
#include "system/system.h"

class SysvarTest : public ::testing::Test {
	static System::Instance *i_system;
	static void SetUpTestCase() {
		i_system = new System::Instance(1);
	}
	
	static void TearDownTestCase() {
		delete i_system;
		i_system = nullptr;
	}

};

//-----------------------------------------------------------------------------
TEST( SysvarTest, Test1 ) {

	auto test1 = System::Variables::Create( "test1", "foo", "foo desc" );
	auto test2 = System::Variables::Create( "test2", "5",   "bar desc" );
	auto test3 = System::Variables::Create( "test3", "5.5", "eee desc" );

	EXPECT_EQ( "test1", test1.Name() );
	EXPECT_EQ( "test2", test2.Name() );
	EXPECT_EQ( "eee desc", test3.Description() );

	EXPECT_EQ( 0, test1.GetInt() );
	EXPECT_EQ( 0.0, test1.GetFloat() );
	EXPECT_EQ( "foo", test1.GetString() );
	EXPECT_EQ( "foo", test1.GetCString() );
	
	EXPECT_EQ( 5,   test2.GetInt() );
	EXPECT_EQ( 5.0, test2.GetFloat() );
	EXPECT_EQ( "5", test2.GetString() );
	EXPECT_EQ( "5", test2.GetCString() );

	EXPECT_EQ( 5,   test3.GetInt() );
	EXPECT_EQ( 5.5, test3.GetFloat() );
	EXPECT_EQ( "5.5", test3.GetString() );
	EXPECT_EQ( "5.5", test3.GetCString() );
}
