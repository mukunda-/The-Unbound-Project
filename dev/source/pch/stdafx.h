//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

#pragma once
 

#ifdef _WIN32
#define TARGET_WINDOWS
#else
#define TARGET_LINUX
#endif

#if PROJECT_CLIENT
#include "pch_client.h"

#elif PROJECT_MASTER
#include "pch_master.h"

#elif PROJECT_NODE
#include "pch_node.h"

#elif PROJECT_AUTH

#include "pch_auth.h"

#elif PROJECT_TEST

#include "pch_test.h"

#else

#error PROJECT IS NOT DEFINED.

#endif

