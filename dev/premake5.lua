function CreateProject( p_name, p_project, p_kind )
	solution( p_name )
	local project_path = "projects/" .. p_name
	location( project_path )
	configurations { "Debug", "Release" }
	
	project( p_name )
	kind( p_kind )
	language "C++"
	
	debugenvs "PATH=$(Path);$(ProjectDir)../../../env/"
	debugdir "../env"
	defines { "PROJECT_" .. p_project }
	
	pchsource "source/pch/stdafx.cpp"
	pchheader "stdafx.h"
	
	-- C++ PCH is not compatible with C files.
	filter { "files:**.c" }
		flags {"NoPCH"}
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
	
	configuration "Debug"
		defines {"_DEBUG"}
		flags {"Symbols"}
		
		libdirs {
			"$(DEVPATH)/libs/debug",
			"$(DEVPATH)/libs/release"
		}
		objdir (project_path .. "/Debug")
	configuration "Release"
		defines {"NDEBUG"}
		optimize "On" 
		
		libdirs {
			"$(DEVPATH)/libs/release"
		}
		objdir (project_path .. "/Release")
	configuration {}
	
	return project_path
end

local project_path = CreateProject( "client", "CLIENT", "WindowedApp" )
	defines { "CLIENT" }
	
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
	
project_path = CreateProject( "master", "MASTER", "ConsoleApp" )
	defines { "SERVER" }
	
	files { 
		"source/pch/*.cpp",
		"source/io/*.cpp",
		"source/network/*.cpp",
		"source/system/*.cpp",
		"source/system/server/*.cpp",
		"source/util/*.cpp",
		"source/mem/*.cpp",
		"source/main_master.cpp",
		
		"protocol/compiled/**.cc"
	}
project_path = CreateProject( "node", "NODE", "ConsoleApp" )
	defines { "SERVER" }
	
	files { 
		"source/pch/*.cpp",
		"source/io/*.cpp",
		"source/network/*.cpp",
		"source/system/*.cpp",
		"source/system/server/*.cpp",
		"source/util/*.cpp",
		"source/mem/*.cpp",
		"source/main_node.cpp",
		
		"protocol/compiled/**.cc"
	}
	