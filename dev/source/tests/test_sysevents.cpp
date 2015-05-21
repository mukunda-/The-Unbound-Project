//===========================  The Unbound Project  =========================//
//                                                                           //
//========= Copyright © 2015, Mukunda Johnson, All rights reserved. =========//

#include <stdafx.h>
#include "system/system.h"
#include "system/events/event.h"
#include "systemtests.h"

///////////////////////////////////////////////////////////////////////////////
namespace Tests {

int SystemTests::event_test;

///////////////////////////////////////////////////////////////////////////////
namespace {

///////////////////////////////////////////////////////////////////////////////
class MyEvent1 : public System::Event {
	
	int m_value;

public:
	SYSTEM_DEFINE_EVENT( 0xFF0001, "TEST_EVENT_1", 0 )

	MyEvent1( int value ) : SYSTEM_EVENT_INIT, m_value(value) {}

	int Value() const { return m_value; }
};

///////////////////////////////////////////////////////////////////////////////
class MyEvent2 : public System::Event {
	
	int m_value;

public:
	SYSTEM_DEFINE_EVENT( 0xFF0002, "TEST_EVENT_2", 0 )

	MyEvent2( int value ) : SYSTEM_EVENT_INIT, m_value(value) {}

	int Value() const { return m_value; }
};

}

///////////////////////////////////////////////////////////////////////////////
void OutsideEventHandler( System::Event &e ) {
	SystemTests::event_test = e.Cast<MyEvent1>().Value() + 5;
}

///////////////////////////////////////////////////////////////////////////////
class EventHookingClass {

	std::vector<System::EventHookPtr> m_hooks;

public:
	EventHookingClass() {
		m_hooks.push_back( System::HookEvent<MyEvent1>( 
				&EventHookingClass::OnEvent, this ));
		m_hooks.push_back( System::HookEvent<MyEvent2>( 
				&EventHookingClass::OnEvent2, this ));
	}

	void OnEvent( System::Event &e ) {
		SystemTests::event_test = e.Cast<MyEvent1>().Value() + 15;
	}

	void OnEvent2( System::Event &e ) {
		SystemTests::event_test += e.Cast<MyEvent2>().Value() + 3;
	}

};

///////////////////////////////////////////////////////////////////////////////
TEST_F( SystemTests, EventTests ) {
	System::RegisterEvent<MyEvent1>();
	System::RegisterEvent<MyEvent2>();
	
	// global hook
	auto hook1 = System::HookEvent<MyEvent1>( OutsideEventHandler );

	// send some events and see if we received them with 
	// the OutsideEventHandler
	System::SendEvent( MyEvent1( 3 ));
	EXPECT_EQ( event_test, 3+5 );

	System::SendEvent( MyEvent1( 4 ));
	EXPECT_EQ( event_test, 4+5 );

	hook1.reset();

	// this should be ignored, since we deleted the event hook.
	System::SendEvent( MyEvent1( 5 )); 
	EXPECT_EQ( event_test, 4+5 );

	// this will hook the event again.
	auto ehc = std::make_shared<EventHookingClass>();

	// send some events and see if we recieved them with the
	// EventHookingClass
	System::SendEvent( MyEvent1( 3 ));
	EXPECT_EQ( event_test, 3+15 );

	System::SendEvent( MyEvent1( 5 ));
	EXPECT_EQ( event_test, 5+15 );
	
	// rehook the outside one again, this should now overwrite
	// the values from the EventHookingClass
	hook1 = System::HookEvent<MyEvent1>( OutsideEventHandler );
	
	System::SendEvent( MyEvent1( 3 ));
	EXPECT_EQ( event_test, 3+5 );

	System::SendEvent( MyEvent1( 5 ));
	EXPECT_EQ( event_test, 5+5 );

	// and reset and try once more..
	hook1.reset();
	System::SendEvent( MyEvent1( 3 ));
	EXPECT_EQ( event_test, 3+15 );

	System::SendEvent( MyEvent1( 5 ));
	EXPECT_EQ( event_test, 5+15 );

	// some different events
	System::SendEvent( MyEvent2( 5 ));
	EXPECT_EQ( event_test, 28 );
	System::SendEvent( MyEvent2( 5 ));
	EXPECT_EQ( event_test, 36 );

	// unhook!
	ehc.reset();

	System::SendEvent( MyEvent1( 53 ));
	System::SendEvent( MyEvent2( 53 ));

	EXPECT_EQ( event_test, 36 );

	// all good!
}

///////////////////////////////////////////////////////////////////////////////
}