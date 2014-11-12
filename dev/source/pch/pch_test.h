//===========================  The Unbound Project  =========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

#pragma once

#define UB_SERVER

#pragma comment( lib, "mysqlcppconn.lib" )
#pragma comment( lib, "pdcurses.lib" )
#pragma comment( lib, "libprotobuf.lib" )

#ifdef _DEBUG
	#pragma comment( lib, "libyaml-cppmdd.lib" )
#else
	#pragma comment( lib, "libyaml-cppmd.lib" )
#endif

//- --- ----


