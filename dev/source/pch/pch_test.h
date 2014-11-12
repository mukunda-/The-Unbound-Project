//===========================  The Unbound Project  =========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

#pragma once

#define UB_SERVER

#define WIN32_LEAN_AND_MEAN 

#pragma comment(lib, "mysqlcppconn.lib")
#pragma comment(lib, "pdcurses.lib")
#pragma comment( lib, "libprotobuf.lib" )

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
#include <boost/asio/posix/stream_descriptor.hpp>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <istream>
#include <ostream>

#include <mysql_driver.h>	
#include <mysql_connection.h>	
#include <cppconn/statement.h>
#include <cppconn/exception.h>

#include <yaml-cpp/yaml.h>

#include <unordered_map>

#ifdef TARGET_WINDOWS
#define STDIN_FILENO 0
#else
#include <unistd.h>
#endif

//- --- ----

#include "google_protobuf.h"
