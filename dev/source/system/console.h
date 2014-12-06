//===========================  The Unbound Project  =========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

#pragma once

#include "util/feed.h"

namespace System { namespace Console {



/// ---------------------------------------------------------------------------
/// Execute a command.
///
void Execute( const char *command_string );

/// ---------------------------------------------------------------------------
/// Execute a script file
///
/// \param file Path to script file, relative to game contents folder.
///
bool ExecuteScript( const char *file );

}}
