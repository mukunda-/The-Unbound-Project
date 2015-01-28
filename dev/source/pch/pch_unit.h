//===========================  The Unbound Project  =========================//
//                                                                           //
//========= Copyright © 2015, Mukunda Johnson, All rights reserved. =========//

#pragma once
   
#include <gtest/gtest.h>
#include <gtest/gtest-spi.h>

#pragma comment( lib, "mysqlcppconn.lib" )
#pragma comment( lib, "ssleay32.lib" )
#pragma comment( lib, "libeay32.lib" )

#ifdef _DEBUG
	#pragma comment( lib, "libyaml-cppmdd.lib" )
#else
	#pragma comment( lib, "libyaml-cppmd.lib" )
#endif


//- --- ----


