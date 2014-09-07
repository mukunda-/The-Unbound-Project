//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//

#include "stdafx.h"

#define VERSION "DEV1.0"

#define WINDOW_TITLE ("ARCHUBOS SERVER (NODE) " VERSION)

int main() {

	System::Console::Init();
	System::Console::SetTitle( WINDOW_TITLE );
	System::Console::Update();

	char input[256];
	while( true ) {
		System::Console::GetInput( input, sizeof input );
		if( input[0] == 27  ) break;

		System::Console::Print( "\n>>> %s", input );
		System::Console::Update();
		

		
	}

	System::Console::End();
	return 0;

}
