//==========================  The Unbound Project  ==========================//
//                                                                           //
//========= Copyright � 2014, Mukunda Johnson, All rights reserved. =========//

#pragma once


#define WIN32_LEAN_AND_MEAN 
#pragma comment(lib, "pdcurses.lib")
#include <curses.h>
#include <panel.h>
#include <boost/asio.hpp>
#include <boost/asio/posix/stream_descriptor.hpp>
#include <boost/array.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/thread.hpp>