//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//

#include <stdafx.h>
#include "test.h"
#include "system/console.h"

//-----------------------------------------------------------------------------
namespace Tests {

	//-------------------------------------------------------------------------
	Test::Test( std::string name ) : 
			m_name(name) {
		
	}

	//-------------------------------------------------------------------------
	bool Test::Run() {
		
		System::Console::Print( "Running TEST: \"%s\"", m_name.c_str() );

		bool passed = Execute();

		System::Console::Print( "TEST %s.", passed ? "PASSED" : "FAILED" );

		return passed;
	}
}