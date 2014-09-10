solution "game"
local PROJECT_PATH = "projects/game"
location (PROJECT_PATH)
configurations { "Debug", "Release" }

project "game"
	kind "WindowedApp"
	language "C++"
	-- do we need this?:
	debugenvs "PATH=$(Path);$(ProjectDir)../../../env/"
	debugdir "../env"
	defines { "PROJECT=GAME" }
	
	pchsource "source/pch/stdafx.cpp"
	pchheader "stdafx.h"
	
	-- C++ PCH is not compatible with C files.
	filter { "files:**.c" }
		flags {"NoPCH"}
		
	-- Don't use PCH for generated protocol files.
	-- Ignore google warnings (explained in google protobuf docs)
	filter { "files:protocol/compiled/**.cc" }
		flags {"NoPCH"}
		warnings "Off"
	filter {}
	
	
	libdirs {
		"$(BOOST_ROOT)/stage/lib",
		"$(DEVPATH)/glew/lib/Release/Win32",
		"$(DEVPATH)/SDL203/lib/x86",
		"$(DEVPATH)/freetype/objs/win32/vc2010"
	}
	
	files { 
		"source/game/*.cpp",
		"source/graphics/*.cpp",
		"source/graphics/gui/*.cpp",
		"source/io/*.cpp",
		"source/mem/*.cpp",
		"source/network/*.cpp",
		"source/system/*.cpp",
		"source/util/*.cpp",
		"source/video/*.cpp",
		"source/pch/*.cpp",
		"source/main_game.cpp",
		
		"libsource/stb_image.c",
		
		"protocol/compiled/**.cc"
	}
	
	includedirs {
		"source/",
		"libsource/",
		"$(BOOST_ROOT)",
		"source/pch",
		"$(DEVPATH)/libs/include",
		"protocol/compiled"
	}
	
	flags { "WinMain" }
	defines { "WIN32", "_WINDOWS" }
	linkoptions { "/nodefaultlib:msvcrt.lib" }
	
	configuration "Debug"
		defines {"_DEBUG"}
		flags {"Symbols"}
		
		libdirs {
			"$(DEVPATH)/libs/debug",
			"$(DEVPATH)/libs/release"
		}
		objdir (PROJECT_PATH .. "/Debug")
	configuration "Release"
		defines {"NDEBUG"}
		optimize "On" 
		
		libdirs {
			"$(DEVPATH)/libs/release"
		}
		objdir (PROJECT_PATH .. "/Release")
