//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//

#pragma once
 

#if PROJECT_CLIENT
#include "pch_client.h"

#elif PROJECT_MASTER
#include "pch_master.h"

#elif PROJECT_NODE
#include "pch_node.h"

#else

#error PROJECT IS NOT DEFINED.

#endif

