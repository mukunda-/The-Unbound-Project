
#pragma once


#define WIN32_LEAN_AND_MEAN
#define _CRT_SECURE_NO_WARNINGS

#pragma comment(lib, "pdcurses.lib")

#include <curses.h>
#include <panel.h>

#include <boost/asio.hpp>
#include <boost/array.hpp>

#include "mem/memorylib.h"
#include "util/KeyValues.h"
#include "util/stringles.h"
#include "util/LinkedList.h"

#include "system/ServerConsole.h"

#include "util/Trie.h"

#include "network/connection.h"
#include "network/nwcore.h"
#include "network/resolver.h"