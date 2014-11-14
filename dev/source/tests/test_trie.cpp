//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

#include <stdafx.h>
#include "trie.h"
#include "util/trie.h"
#include "system/console.h"

//-----------------------------------------------------------------------------
namespace Test {
	Trie::Trie() : Test( "Trie Test" ) {}

	//-------------------------------------------------------------------------
	void Trie::Execute() {
		using System::Console::Print;
		
		{
			Print( "test1..." );
			Util::Trie<int> test;
			
		}
	}
}