//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//

#pragma once

#ifndef TITLESCREEN_H
#define TITLESCREEN_H

#include <Windows.h>

namespace TitleScreen {

void Start();

/// events

void OnExitButton( void *source, UINT msg, WPARAM wParam, LPARAM lParam);
void pooper( void *source, UINT msg, WPARAM wParam, LPARAM lParam);

}

#endif
