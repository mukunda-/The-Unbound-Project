//===========================  The Unbound Project  =========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

#include <stdafx.h>
#include "system/variables.h"
#include "system/system.h"
#include "system/commands.h"
#include "systemtests.h"

///////////////////////////////////////////////////////////////////////////////
namespace Tests {

///////////////////////////////////////////////////////////////////////////////
System::Main* SystemTests::i_system = nullptr;
int SystemTests::counter;

///////////////////////////////////////////////////////////////////////////////
namespace {

	void OnChanged1( System::Variable &v ) {
		EXPECT_EQ( &v, System::Variables::Find( "test1" ));
		SystemTests::counter++;
	}

	void OnChanged2( System::Variable &v ) {
		EXPECT_EQ( &v, System::Variables::Find( "test2" ));
		SystemTests::counter++;
	}

	void OnChanged3( System::Variable &v ) {
		EXPECT_EQ( &v, System::Variables::Find( "test3" ));
		SystemTests::counter++;
	}
}

///////////////////////////////////////////////////////////////////////////////
TEST_F( SystemTests, VariableTests ) {

	auto &test1 = System::Variables::Create( "test1", "foo", "foo desc" );
	auto &test2 = System::Variables::Create( "test2", "5",   "bar desc" );
	auto &test3 = System::Variables::Create( "test3", "5.5", "eee desc" );

	System::Variables::Create( "test1", "should not be used", "try to remake variable", 2 );
	System::Variables::Create( "test2", "should not be used", "try to remake variable", 2 );
	System::Variables::Create( "test3", "should not be used", "try to remake variable", 2 );
	
	///////////////////////////////////////////////////////
	// hooking
	///////////////////////////////////////////////////////
	int hook1  = test1.HookChange( OnChanged1 );
	int hook2  = test2.HookChange( OnChanged2 );
	int hook31 = test3.HookChange( OnChanged3 );
	int hook32 = test3.HookChange( OnChanged3 );
	ExpectHooksSize( test1, 1 );
	ExpectHooksSize( test2, 1 );
	ExpectHooksSize( test3, 2 );

	///////////////////////////////////////////////////////
	// name
	///////////////////////////////////////////////////////
	EXPECT_EQ( "test1", test1.Name() );
	EXPECT_EQ( "test2", test2.Name() );
	EXPECT_EQ( "eee desc", test3.Description() );

	///////////////////////////////////////////////////////
	// initialized previous value
	///////////////////////////////////////////////////////
	EXPECT_EQ( 0, test1.PreviousInt() );
	EXPECT_EQ( 0.0, test1.PreviousFloat() );
	EXPECT_EQ( "", test1.PreviousString() );

	///////////////////////////////////////////////////////
	// values and conversions
	///////////////////////////////////////////////////////
	EXPECT_EQ( 0, test1.GetInt() );
	EXPECT_EQ( 0.0, test1.GetFloat() );
	EXPECT_EQ( "foo", test1.GetString() );
	EXPECT_STREQ( "foo", test1.GetCString() );
	
	EXPECT_EQ( 5,   test2.GetInt() );
	EXPECT_EQ( 5.0, test2.GetFloat() );
	EXPECT_EQ( "5", test2.GetString() );
	EXPECT_STREQ( "5", test2.GetCString() );

	EXPECT_EQ( 5,   test3.GetInt() );
	EXPECT_EQ( 5.5, test3.GetFloat() );
	EXPECT_EQ( "5.5", test3.GetString() );
	EXPECT_STREQ( "5.5", test3.GetCString() );

	// variables have not changed yet, counter should be 0
	EXPECT_EQ( SystemTests::counter, 0 );

	///////////////////////////////////////////////////////
	// variable changes
	///////////////////////////////////////////////////////
	test1.SetInt( 2 );

	// counter increments in OnChanged
	EXPECT_EQ( SystemTests::counter, 1 );

	// verify new values
	EXPECT_EQ( 2, test1.GetInt() );
	EXPECT_EQ( 2.0, test1.GetFloat() );
	EXPECT_EQ( "2", test1.GetString() );

	// verify previous values
	EXPECT_EQ( 0, test1.PreviousInt() );
	EXPECT_EQ( 0.0, test1.PreviousFloat() );
	EXPECT_EQ( "foo", test1.PreviousString() );

	///////////////////////////////////////////////////////
	test1.UnhookChange( hook1 );
	test1.SetFloat( 1.25 );

	// unhooked, counter should not change
	EXPECT_EQ( SystemTests::counter, 1 );

	// verify values
	EXPECT_EQ( 1, test1.GetInt() );
	EXPECT_EQ( 1.25, test1.GetFloat() );
	EXPECT_EQ( "1.25", test1.GetString() );

	EXPECT_EQ( 2, test1.PreviousInt() );
	EXPECT_EQ( 2.0, test1.PreviousFloat() );
	EXPECT_EQ( "2", test1.PreviousString() );

	///////////////////////////////////////////////////////
	hook1 = test1.HookChange( OnChanged1 );
	test1.SetString( "5.5" );

	// hooked again, counter should change
	EXPECT_EQ( SystemTests::counter, 2 );

	// verifyvalues
	EXPECT_EQ( test3.GetInt(), test1.GetInt() );
	EXPECT_EQ( test3.GetFloat(), test1.GetFloat() );
	EXPECT_EQ( test3.GetString(), test1.GetString() );

	EXPECT_EQ( 1, test1.PreviousInt() );
	EXPECT_EQ( 1.25, test1.PreviousFloat() );
	EXPECT_EQ( "1.25", test1.PreviousString() );

	// test2 should equal test1
	EXPECT_EQ( test2.GetInt(), test1.GetInt() );
	EXPECT_NE( test2.GetFloat(), test1.GetFloat() );
	EXPECT_NE( test2.GetString(), test1.GetString() );

	///////////////////////////////////////////////////////
	test1.SetString( "hello" );

	// onemore test
	EXPECT_EQ( SystemTests::counter, 3 );
	EXPECT_EQ( 0, test1.GetInt() );
	EXPECT_EQ( 0.0, test1.GetFloat() );
	EXPECT_EQ( "hello", test1.GetString() );

	test3.SetString( "test" );
	EXPECT_EQ( SystemTests::counter, 5 );
	test3.SetString( "test" );
	EXPECT_EQ( SystemTests::counter, 5 );

	EXPECT_EQ( test1.GetInt(), test3.GetInt() );

	///////////////////////////////////////////////////////
	// verify hook behavior
	///////////////////////////////////////////////////////
	try {
		test1.UnhookChange( 25 );
		FAIL();
	} catch( std::runtime_error& ) {}

	ExpectHooksSize( test1, 1 );
	test1.UnhookChange( hook1 );
	ExpectHooksSize( test1, 0 );

	try {
		test1.UnhookChange( hook1 );
		FAIL();
	} catch( std::runtime_error& ) {}

	ExpectHooksSize( test1, 0 );

	try {
		test1.UnhookChange( hook1 );
		FAIL();
	} catch( std::runtime_error& ) {}

	ExpectHooksSize( test1, 0 );

	///////////////////////////////////////////////////////
	ExpectHooksSize( test2, 1 );
	test2.UnhookChange( hook2 );
	try {
		test2.UnhookChange( hook2 );
		FAIL();
	} catch( std::runtime_error& ) {}
	
	ExpectHooksSize( test2, 0 );

	///////////////////////////////////////////////////////
	ExpectHooksSize( test3, 2 );
	test3.UnhookChange( hook31 );
	ExpectHooksSize( test3, 1 );
	test3.UnhookChange( hook32 );
	ExpectHooksSize( test3, 0 );

	try {
		test3.UnhookChange( hook31 );
		FAIL();
	} catch( std::runtime_error& ) {}

	ExpectHooksSize( test3, 0 );

	try {
		test3.UnhookChange( hook32 );
	} catch( std::runtime_error& ) {}

	ExpectHooksSize( test3, 0 );

	///////////////////////////////////////////////////////
	// delete/recreate variables
	///////////////////////////////////////////////////////
	EXPECT_TRUE( System::Variables::Delete( "test1" ));
	
	auto &test4 = System::Variables::Create( "test1", "bar", "foo desc2" );
	EXPECT_EQ( "bar", test4.GetString() );
	EXPECT_EQ( "foo desc2", test4.Description() );

}

///////////////////////////////////////////////////////////////////////////////
TEST_F( SystemTests, CommandTest ) {

	auto cmd = System::Command::Create( "test1", "test command 1", 
		[]( Util::ArgString &args ) {
			
		}
	);

	//System::TryExecuteCommand( 
}

}