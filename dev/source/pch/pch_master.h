//===========================  The Unbound Project  =========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

#pragma once


#define WIN32_LEAN_AND_MEAN 

#pragma comment(lib, "mysqlcppconn.lib")
#pragma comment(lib, "pdcurses.lib")

#ifdef _DEBUG
	#pragma comment( lib, "libyaml-cppmdd.lib" )
#else
	#pragma comment( lib, "libyaml-cppmd.lib" )
#endif

#include <curses.h>
#include <panel.h>

#include <boost/asio.hpp>
#include <boost/array.hpp>

#include <Eigen/Dense>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <thread>

#include <mysql_driver.h>	
#include <mysql_connection.h>	
#include <cppconn/statement.h>

#include <yaml-cpp/yaml.h>
