-------------------------------------------------------------------------------
-- win64 lua buildscript
-------------------------------------------------------------------------------

-------------------------------------------------------------------------------
-- library
-------------------------------------------------------------------------------
solution       "library" 
configurations { "Static Debug", "Static Release", "DLL Debug", "DLL Release" }
platforms      { "x64" }
architecture   "x64"

includedirs { "config", "src" }
defines     { "WIN32", "_WIN32", "_WINDOWS" }
defines     { "WIN64", "_WIN64" }

project  "lua53"
language "C"
location "build/library"

-------------------------------------------------------------------------------
configuration "Static Debug"
	defines    { "_DEBUG"  }
	flags      { "Symbols" }
	targetdir  "build/debug"
	targetname "lua53s"
	kind       "StaticLib"
	 
configuration  "Static Release" 
	defines    { "NDEBUG" }
	optimize   "On" 
	targetdir  "build/release"
	targetname "lua53s"
	kind       "StaticLib"
	
configuration "DLL Debug"
	defines    { "_DEBUG"  }
	flags      { "Symbols" }
	targetdir  "build/debug"
	targetname "lua53"
	kind       "SharedLib"
	 
configuration  "DLL Release" 
	defines    { "NDEBUG" }
	optimize   "On" 
	targetdir  "build/release"
	targetname "lua53"
	kind       "SharedLib"
	
configuration {}

-------------------------------------------------------------------------------
files {
	"src/lapi.c",    "src/lcode.c",    "src/lctype.c",   "src/ldebug.c"   ;
	"src/ldo.c",     "src/ldump.c",    "src/lfunc.c",    "src/lgc.c"      ; 
	"src/llex.c",    "src/lmem.c",     "src/lobject.c",  "src/lopcodes.c" ;
	"src/lparser.c", "src/lstate.c",   "src/lstring.c",  "src/ltable.c"   ;
	"src/ltm.c",     "src/lundump.c",  "src/lvm.c",      "src/lzio.c"     ;
	"src/lauxlib.c", "src/lbaselib.c", "src/lbitlib.c",  "src/lcorolib.c" ;
	"src/ldblib.c",  "src/liolib.c",   "src/lmathlib.c", "src/loslib.c"   ;
	"src/lstrlib.c", "src/ltablib.c",  "src/lutf8lib.c", "src/loadlib.c"  ;
	"src/linit.c"                                                         ;
	
	"**.h"
}

-------------------------------------------------------------------------------
-- binaries
-------------------------------------------------------------------------------

solution       "bins"
configurations { "Debug", "Release" }
platforms      { "x64" }
architecture   "x64"

includedirs    { "config", "src" }
defines        { "WIN32", "_WIN32", "_WINDOWS" }
defines        { "WIN64", "_WIN64" }
kind           "ConsoleApp"
language       "C"


configuration "Debug"
	defines    { "_DEBUG"  }
	flags      { "Symbols" }
	targetdir  "build/debug"
	libdirs    { "build/debug" }
configuration "Release" 
	defines    { "NDEBUG" }
	optimize   "On" 
	targetdir  "build/release"
	libdirs    { "build/release" }
configuration {}
	
links { "lua53s.lib" } 

-------------------------------------------------------------------------------
-- Lua Interpreter
-------------------------------------------------------------------------------
project  "lua" 
location "build/interpreter"
 
files { "src/lua.c" }
links { "lua53s.lib" } 

-------------------------------------------------------------------------------
-- Lua Compiler
-------------------------------------------------------------------------------
project  "luac" 
location "build/compiler"
 
files { "src/luac.c" }
links { "lua53s.lib" }
