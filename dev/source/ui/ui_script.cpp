//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2015, Mukunda Johnson, All rights reserved. =========//

// access to ui interface from lua

#include "stdafx.h"
#include "ui.h"

namespace Ui {

//-----------------------------------------------------------------------------
namespace {

void UI_Create() {

}

//-----------------------------------------------------------------------------
} // namespace

void Instance::SetupScripting() {
	lua_register(  UI_Create
}

} // namespace Ui

