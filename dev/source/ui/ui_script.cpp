//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright � 2015, Mukunda Johnson, All rights reserved. =========//

// access to ui interface from lua

#include "stdafx.h"
#include "ui.h"

namespace Ui {

//-----------------------------------------------------------------------------
namespace {

int UI_Create( lua_State *L ) {

}

//-----------------------------------------------------------------------------
} // namespace

void Instance::SetupScripting() {
	lua_register( System::LS(), "UI_Create", UI_Create );
}

} // namespace Ui

