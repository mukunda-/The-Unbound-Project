//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//

#pragma once

#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "OpenGL32.lib")
#pragma comment(lib, "GLu32.lib")
#pragma comment(lib, "sdl2.lib")
#pragma comment( lib, "freetype246.lib" )
#pragma comment( lib, "libprotobuf.lib" )

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <algorithm>
#include <iostream>
#include <regex>
#include <stdio.h>
#include <stdlib.h> 
#include <time.h>
#include <assert.h>
#include <Eigen/Dense>
#include <stdint.h>
#include <SDL.h>
#include "io/stb_image.h"

#include <GL/glew.h>
#include <GL/GL.h>
#include <gl/GLU.h>

#include <vector>
#include <fstream>
#include <boost/shared_array.hpp>
#include <stdexcept>

#include <io.h>
#include <fcntl.h>
#include <Eigen/Dense>

#include <boost/thread.hpp>

#include "ft2build.h"
#include "freetype/freetype.h"

#include <boost/asio.hpp>