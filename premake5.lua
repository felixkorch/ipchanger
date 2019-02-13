require("premake-qt/qt")
require("paths")
local qt = premake.extensions.qt

workspace "IPChanger"
	architecture "x64"
	startproject "Gui"

	configurations
	{
		"Debug",
		"Release"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

project "IPChanger"

	local dir = "ipchanger"

	location (dir)
	kind "StaticLib"
	staticruntime "Off"
	language "C++"
	cppdialect "C++17"

	pchheader "ippch.h"
	pchsource (dir .. "/src/ippch.cpp")

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		(dir .. "/**.cpp"),
		(dir .. "/**.h")
	}

	includedirs
	{
		(dir .. "/**")
	}

	filter "system:windows"
		characterset "MBCS"
		systemversion "latest"
		sysincludedirs { "C:/local/include/*", "C:/*" }
		links { "ws2_32" }
		excludes { dir .. "/src/ipchanger/platform/unix/**.cpp" }

	filter "system:not windows"
		links { "stdc++fs" }
		excludes { dir .. "/src/ipchanger/platform/windows/**.cpp" }

	filter "configurations:Debug"
		defines { "DEBUG" }
		symbols "On"

	filter "configurations:Release"
		defines { "NDEBUG" }
		optimize "On"


project "Gui"

	local dir = "gui"

	location (dir)
	kind "WindowedApp"
	staticruntime "Off"
	language "C++"
	cppdialect "C++17"

	qt.enable()
	qtmodules { "core", "gui", "widgets" }
	qtprefix "Qt5"


	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		(dir .. "/src/**.cpp"),
		(dir .. "/src/**.h"),
		(dir .. "/src/**.ui"),
		(dir .. "/src/**.qrc")
	}

	includedirs
	{
		(dir .. "/**"),
		"ipchanger/**"
	}

	links
	{
		"IPChanger"
	}

	filter "system:windows"
		systemversion "latest"
		sysincludedirs { "C:/local/include/*", "C:/*" }
		qtpath (QtPaths.qtdir)
		links { "ws2_32" }
		targetname "IPChanger"

	filter "system:not windows"
		links { "stdc++fs" }
		qtbinpath (QtPaths.qtbin)
		qtlibpath (QtPaths.qtlib)
		qtincludepath (QtPaths.qtinclude)
		buildoptions { "-fPIC" }
		targetname "ipchanger"

	filter "configurations:Debug"
		defines { "DEBUG" }
		symbols "On"

	filter "configurations:Release"
		defines { "NDEBUG" }
		runtime "Release"
		optimize "On"

project "Sandbox"

	local dir = "sandbox"

	location (dir)
	kind "ConsoleApp"
	staticruntime "Off"
	language "C++"
	cppdialect "C++17"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		(dir .. "/src/**.cpp"),
		(dir .. "/src/**.h")
	}

	includedirs
	{
		(dir .. "/**"),
		"ipchanger/**"
	}

	links
	{
		"IPChanger"
	}

	filter "system:windows"
		systemversion "latest"
		sysincludedirs { "C:/local/include/*", "C:/*" }
		links { "ws2_32" }

	filter "system:not windows"
		links { "stdc++fs" }
		targetname "sandbox"

	filter "configurations:Debug"
		defines { "DEBUG" }
		symbols "On"

	filter "configurations:Release"
		defines { "NDEBUG" }
		runtime "Release"
		optimize "On"