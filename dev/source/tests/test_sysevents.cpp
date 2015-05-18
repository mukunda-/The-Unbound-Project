//===========================  The Unbound Project  =========================//
//                                                                           //
//========= Copyright © 2015, Mukunda Johnson, All rights reserved. =========//

#include <stdafx.h>
#include "system/system.h"
#include "system/event.h"
#include "systemtests.h"

///////////////////////////////////////////////////////////////////////////////
namespace {

class MyEvent1 : public System::Event {
	
	int m_value;

public:
	SYSTEM_DEFINE_EVENT( 0xFF0000, "TEST_EVENT_1", 0 )
	
	MyEvent1( int value ) : SYSTEM_EVENT_INIT, m_value(value) {
	}

};

}

///////////////////////////////////////////////////////////////////////////////
TEST_F( SystemTests, EventTests ) {
	System::RegisterEvent<MyEvent1>();
}
