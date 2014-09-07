//============================  The Unbound Project  ==========================//
//                                                                             //
//========== Copyright © 2014, Mukunda Johnson, All rights reserved. ==========//

#pragma once


#ifdef GAME_CLIENT

//#define WIN32_LEAN_AND_MEAN

#include <WinSock2.h>
#include <ws2tcpip.h>

#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <fstream>
#include <math.h>
#include <fcntl.h>
#include <io.h>
#include <iostream>

#include <sdl/SDL.h>
#include <sdl/SDL_audio.h>

#include <cml/cml.h>
//#include <fbxsdk.h>

#include <vector>
#include <string>
#include <queue>

#include <dinput.h>

#include "D3DX10math.h"

#include <ft2build.h>
#include FT_FREETYPE_H

//#include <fbxsdk.h>

#include <GL/glew.h>
#include <gl/gl.h>
#include <gl/glu.h>

#include "misc/types.h"

#include "glext.h"
#include "video/shader.h"
#include "graphics/rasterfont.h"
#include "video/textures.h"
#include "video/VertexArrayObject.h"
#include "video/vertexbuffer.h"
#include "video/vertexformats.h"
#include "video/video.h"
#include "video/Surface.h"
#include "video/console.h"

#include "video/shaders/WorldShader.h"
#include "video/shaders/HUDShader.h"
#include "video/shaders/ObjectShader.h"
#include "video/shaders/ScreenShader.h"
#include "video/shaders/BackdropShader.h"
#include "video/shaders/SurfaceMixer.h"
#include "video/shaders/CloudShader.h"

//#include "world/planet.h"
#include "graphics/worldrender.h"
 
#include "world/Cell.h"
#include "world/CellCodec.h"
#include "world/CellTree.h"
#include "world/CellPool.h"
#include "world/CellShadow.h"
#include "world/CellReader.h"
//#include "world/ChunkTable.h"
#include "world/CommandQueue.h"
#include "world/Context.h"
#include "world/CubeKernel.h"
#include "world/disk.h"
#include "world/Engine.h"
#include "world/Generation.h"
#include "world/JobManager.h"
#include "world/LockTable.h"
#include "world/planetlib.h"
#include "world/planetlib_private.h"
#include "world/RegionFileLock.h"
#include "world/WorldParameters.h"
#include "world/WorldUtil.h"

#include "audio/audio.h"
#include "audio/AudioList.h"
#include "audio/TransferQueue.h"
#include "audio/Sample.h"
#include "audio/Channel.h"

#include "debug/debug_print_bitmap.h"
#include "debug/profiler.h"
/*
#include "game/entity.h"
#include "game/explosion.h"
#include "game/game.h"
#include "game/gametime.h"
#include "game/ingame.h"
#include "game/objects.h"
#include "game/player.h"
#include "game/projectile.h"
#include "game/titlescreen.h"
*/

#include "graphics/backdrop.h"
#include "graphics/clouds.h"
#include "graphics/graphics.h"
#include "graphics/hud.h"
//#include "graphics/models.h"
#include "graphics/particles.h"
#include "graphics/scene.h"
#include "graphics/ScreenEffects.h"
#include "graphics/world_texture.h"

#include "graphics/world/SkylightManager.h"
#include "graphics/world/WorldRenderer.h"
#include "graphics/world/WorldRenderingInstance.h"
#include "graphics/world/WorldRenderingList.h"

#include "input/input.h"

#include "io/BinaryFile.h"
#include "io/bmp.h"
#define STBI_HEADER_FILE_ONLY
#include "stb_image.c"
#include "io/stb_image.h"
#include "io/filebase.h"

#include "tinystr.h"
#include "tinyxml.h"

#include "math/trig.h"

#include "util/LinkedList.h"
#include "misc/random.h"
#include "misc/timestamp.h"

#include "util/hashstring.h"
#include "util/dstring.h"
#include "util/StringTable.h"
#include "util/ScriptParser.h"
#include "util/LinkedList.h"
#include "util/stringles.h"
#include "util/LockList.h"
#include "util/EventWaiter.h"
#include "util/KeyValues.h"

#include <memorylib.h>

#include "system/servermasternode.h"
#include "system/zonemanager.h"



#endif

#ifdef SERVER_MASTER

#include <curses.h>
#include <panel.h>

#include "util/KeyValues.h"
#include "util/stringles.h"

#include "system/ServerConsole.h"

#endif

#ifdef SERVER_NODE 

#include "system/ServerConsole.h"

#endif



#include "globals.h"

