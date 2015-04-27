//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2015, Mukunda Johnson, All rights reserved. ==========//

#pragma once


#pragma comment( lib, "glew64.lib" ) // renamed so it finds the right one!
#pragma comment( lib, "OpenGL32.lib" )
#pragma comment( lib, "GLu32.lib" )
#pragma comment( lib, "sdl2.lib" )
#pragma comment( lib, "libprotobuf.lib" )
#pragma comment( lib, "ssleay32.lib" )
#pragma comment( lib, "libeay32.lib" )

#ifdef _DEBUG
#   pragma comment( lib, "libyaml-cppmdd.lib" )
#   pragma comment( lib, "freetype255d.lib" )
#else
#   pragma comment( lib, "libyaml-cppmd.lib" )
#   pragma comment( lib, "freetype255.lib" )
#endif

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <algorithm>
#include <iostream>
#include <regex>
#include <stdio.h>
#include <stdlib.h> 
#include <time.h>
#include <assert.h>
#include <stdint.h>
#include <SDL.h>
#include "io/stb_image.h"

#include <GL/glew.h>
#include <GL/GL.h>
#include <gl/GLU.h>

#include <vector>
#include <fstream>
#include <stdexcept>

#include <io.h>
#include <fcntl.h>

#include "ft2build.h"
#include "freetype/freetype.h"

#include <boost/asio/high_resolution_timer.hpp>
#include <boost/asio/steady_timer.hpp>