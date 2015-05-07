//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2015, Mukunda Johnson, All rights reserved. =========//

///////////////////////////////////////////////////////////////////////////////
// SANDBOX FOR DEVELOPING SERVER STUFF
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
 
#include "system/system.h"

class Test : public System::Module {
public:
	Test();
};

Test::Test() : Module( "testapp", Levels::USER ) {

};

//-------------------------------------------------------------------------------------------------
int main( int argc, char *argv[] ) {
	System::Main system(4);


	return 0;
} 