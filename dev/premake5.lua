-------------------------------------------------------------------------------
--- Project creation helper
---   p_name: Actual name of project, eg master
---   p_project: Project definition, eg MASTER
---   p_kind: WindowedApp or ConsoleApp 
---
function CreateProject( p_name, p_project, p_kind, p_64bit )
	solution( p_name )
	local project_path = "projects/" .. p_name
	location( project_path )
	configurations { "Debug", "Release" }
	
	if p_64bit then
		platforms { "x64" }
		architecture "x64"
	end
	-------------------------------------------------------
	project( p_name )
	kind( p_kind )
	language "C++"
	
	-- debug environment, point to /env folder ------------
	debugenvs "PATH=$(Path);$(ProjectDir)../../../env/"
	debugdir "../env"
	
	-- project definition, passed in as an argument -------
	defines { "PROJECT_" .. p_project }
	
	-- PCH source files -----------------------------------
	pchsource "source/pch/stdafx.cpp"
	pchheader "stdafx.h"
	
	-- C++ PCH is not compatible with C files. ------------
	-- disable PCH for C files.
	filter { "files:**.c" }
		flags { "NoPCH" }
	filter { "files:protocol/compiled/**.cc" }
		flags { "NoPCH" }
		warnings "Off"
	filter {}
	
	-- PCH memory allocation ------------------------------
	buildoptions { "/Zm130" }
	
	-- library directories --------------------------------
	
	if( p_64bit ) then
		libdirs { 
			"$(BOOST_ROOT)/lib/x64/lib", 
			"$(DEVPATH)/glew/lib/Release/Win64",
			"$(DEVPATH)/SDL203/lib/x64",
			"$(DEVPATH)/freetype-2.5.5/objs/vc2010/x64"
		}
	else
		libdirs { 
			"$(BOOST_ROOT)/stage/lib",
			"$(DEVPATH)/glew/lib/Release/Win32",
			"$(DEVPATH)/SDL203/lib/x86"
			--"$(DEVPATH)/freetype/objs/win32/vc2010" need to update this if needed.
		}
		
	end
	
	
	-- include directories --------------------------------
	includedirs {
		"source/",
		"libsource/",
		"$(BOOST_ROOT)",
		"source/pch",
		"$(DEVPATH)/libs/include",
		"protocol/compiled"
	}
	
	if p_kind == "WindowedApp" then
		flags { "WinMain" }
	end
	defines { "WIN32", "_WINDOWS" }
	linkoptions { "/nodefaultlib:msvcrt.lib" }
	
	-------------------------------------------------------
	if p_64bit then
		libdirs {
			"$(DEVPATH)/libs/vs12/debug/x64",
			"$(DEVPATH)/libs/vs12/release/x64"
		}
	end
		
	-------------------------------------------------------
	configuration { "Release", "x64" }
		libdirs {
			"$(DEVPATH)/libs/vs12/release/x64"
		}
	
	-------------------------------------------------------
	configuration "Debug"
		defines {"_DEBUG"}
		flags {"Symbols"}
		
		libdirs {
			"$(DEVPATH)/libs/vs12/debug/x86",
			"$(DEVPATH)/libs/vs12/release/x86",
			"$(DEVPATH)/libs/debug",
			"$(DEVPATH)/libs/release"
		}
		objdir (project_path .. "/Debug")
		
	-------------------------------------------------------
	configuration "Release"
		defines {"NDEBUG"}
		optimize "On" 
		
		libdirs {
			"$(DEVPATH)/libs/vs12/release/x86",
			"$(DEVPATH)/libs/release"
		}
		objdir (project_path .. "/Release")
	
	-------------------------------------------------------
	configuration {}
	
	return project_path
end

local project_path;

project_path = CreateProject( "client", "CLIENT", "WindowedApp", true )
	defines { "UB_CLIENT" }
	
	files { 
		"source/console/*.cpp",
		"source/asev/*.cpp",
		"source/game/*.cpp",
		"source/graphics/*.cpp",
		"source/graphics/gui/*.cpp",
		"source/io/*.cpp",
		"source/mem/*.cpp",
		"source/mem/arena/*.cpp",
		"source/net/*.cpp",
		"source/system/*.cpp",
		"source/util/*.cpp",
		"source/video/*.cpp",
		"source/pch/*.cpp",
		"source/main_game.cpp",
		
		"libsource/stb_image.c",
		
		"source/**.h",
		
		"protocol/compiled/**.cc"
	}

project_path = CreateProject( "master", "MASTER", "ConsoleApp", true )
	defines { "UB_SERVER" }
	
	files { 
		"source/pch/*.cpp",
		"source/io/*.cpp",
		"source/net/*.cpp",
		"source/system/*.cpp",
		"source/system/server/*.cpp",
		"source/util/*.cpp",
		"source/mem/*.cpp",
		"source/main_master.cpp",
		
		"protocol/compiled/**.cc"
	}

project_path = CreateProject( "node", "NODE", "ConsoleApp", true )
	defines { "UB_SERVER" }
	
	files {
		"source/pch/*.cpp",
		"source/io/*.cpp",
		"source/net/*.cpp",
		"source/system/*.cpp",
		"source/system/server/*.cpp",
		"source/util/*.cpp",
		"source/mem/*.cpp",
		"source/main_node.cpp",
		
		"protocol/compiled/**.cc"
	}

project_path = CreateProject( "auth", "AUTH", "ConsoleApp", true )
	defines {"UB_SERVER"}
	
	files {
		"source/console/*.cpp",
		"source/asev/*.cpp",
		"source/pch/*.cpp",
		"source/db/*.cpp",
		"source/io/*.cpp",
		"source/net/*.cpp",
		"source/system/*.cpp",
		"source/system/server/*.cpp",
		"source/util/*.cpp",
		"source/mem/*.cpp",
		"source/mem/arena/*.cpp",
		"source/programs/auth/*.cpp",
		"source/main_auth.cpp",
		
		"protocol/compiled/**.cc",
		 
		"source/**.h",
		
		"libsource/hash/*.c",
		"libsource/hash/*.cpp",
		"libsource/hash/*.h"
	}

project_path = CreateProject( "test", "TEST", "ConsoleApp", true )
	defines {"UB_TESTING"}
	
	files {
		"source/console/*.cpp",
		"source/pch/*.cpp",
		"source/io/*.cpp",
		"source/net/*.cpp",
		"source/db/*.cpp",
		"source/asev/*.cpp",
		"source/system/*.cpp",
		"source/system/server/*.cpp",
		"source/util/*.cpp",
		"source/mem/*.cpp",
		"source/mem/arena/*.cpp", 
		"source/main_test.cpp",
		
		
		"protocol/compiled/**.cc",
		"source/**.h"
	}
	
project_path = CreateProject( "rxgserv", "RXGSERV", "ConsoleApp", true )
	defines {"UB_SERVER"}
	
	files {
		"source/console/*.cpp",
		"source/pch/*.cpp",
		"source/io/*.cpp",
		"source/net/*.cpp",
		"source/db/*.cpp",
		"source/asev/*.cpp",
		"source/system/*.cpp",
		"source/system/server/*.cpp",
		"source/util/*.cpp",
		"source/mem/*.cpp",
		"source/mem/arena/*.cpp", 
		"source/programs/rxgserv/*.cpp",
		"source/main_rxgserv.cpp",
		
		"protocol/compiled/**.cc",
		"source/**.h"
	}
	 
project_path = CreateProject( "unit", "UNIT", "ConsoleApp", true )
	defines {"UB_TESTING"}
	
	files {
		"source/console/*.cpp",
		"source/pch/*.cpp",
		"source/io/*.cpp",
		"source/net/*.cpp",
		"source/db/*.cpp",
		"source/asev/*.cpp",
		"source/system/*.cpp",
		"source/system/server/*.cpp",
		"source/util/*.cpp",
		"source/mem/*.cpp",
		"source/mem/arena/*.cpp",
		"source/tests/*.cpp",
		"source/main_unit.cpp",
		
		"protocol/compiled/**.cc",
		"source/**.h",
		
		"libsource/hash/*.c",
		"libsource/hash/*.cpp",
		"libsource/hash/*.h"
	}
	
	