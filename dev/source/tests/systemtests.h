//===========================  The Unbound Project  =========================//
//                                                                           //
//========= Copyright � 2014, Mukunda Johnson, All rights reserved. =========//

// fixture for testing the System class and subclasses

#pragma once

namespace Tests {

///////////////////////////////////////////////////////////////////////////////
class SystemTests : public ::testing::Test {
	static System::Main *i_system;


	///////////////////////////////////////////////////////////////////////////
protected:
	static void SetUpTestCase() {
		i_system = new System::Main( 1, System::StartMode::PASS );
		counter = 0;
		event_test = 0;
	}
	
	static void TearDownTestCase() {
		delete i_system;
		i_system = nullptr;
	}
	///////////////////////////////////////////////////////////////////////////
public:
	
	static int counter;
	static int event_test;

	static void ExpectHooksSize( System::Variable &v, size_t size ) {
		EXPECT_EQ( size, v.m_change_handlers.size() );
	}
};

///////////////////////////////////////////////////////////////////////////////
}