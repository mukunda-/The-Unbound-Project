
#pragma once
 
#pragma comment( lib, "mysqlcppconn.lib" )
#pragma comment( lib, "pdcurses.lib" )
#pragma comment( lib, "libprotobuf.lib" )

#ifdef _DEBUG
	#pragma comment( lib, "libyaml-cppmdd.lib" )
#else
	#pragma comment( lib, "libyaml-cppmd.lib" )
#endif