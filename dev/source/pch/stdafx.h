//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright © 2014, Mukunda Johnson, All rights reserved. =========//

#pragma once
 

#ifdef _WIN32
#define TARGET_WINDOWS
#define WIN32_LEAN_AND_MEAN 
#else
#define TARGET_LINUX
#endif

#include <Eigen/Dense>
#include <boost/asio.hpp>
#include <boost/asio/posix/stream_descriptor.hpp>
#include <boost/algorithm/string.hpp>

#include <boost/asio.hpp>
#include <boost/array.hpp>

#include <boost/thread.hpp>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <memory>
#include <atomic>
#include <typeinfo>
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

#include "google_protobuf.h"

#include <curses.h>
#include <panel.h>


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
 
#endif

