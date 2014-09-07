
#pragma once

#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "OpenGL32.lib")
#pragma comment(lib, "GLu32.lib")
#pragma comment(lib, "sdl2.lib")

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <Eigen/Dense>
#include <stdint.h>


#include "video/video.h"
#include "video/vertexbuffer.h"
#include "video/Shader.h"
#include "video/shaders/BackdropShader.h"
#include "video/shaders/linetest.h"

#include <SDL.h>

#include "graphics/vertexformats.h"
 