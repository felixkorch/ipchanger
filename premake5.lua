require( "premake-qt/qt" )
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

	pchheader "pch.h"
	pchsource (dir .. "/src/pch.cpp")

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		(dir .. "/src/**.cpp"),
		(dir .. "/src/**.h")
	}

	includedirs
	{
		(dir .. "/src"),
		(dir .. "/src/**")
	}

	filter "system:windows"
		systemversion "latest"
		sysincludedirs { "C:/local/include/*", "C:/*" }
		links { "ws2_32" }
		excludes { dir .. "/src/ipchanger/system/platform/linux/**.cpp" }

	filter "system:not windows"
		links { "stdc++fs" }
		excludes { dir .. "/src/ipchanger/system/platform/windows/**.cpp" }

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
		(dir .. "/src"),
		(dir .. "/src/**"),
		"ipchanger/src/",
		"ipchanger/src/**"
	}

	links
	{
		"IPChanger"
	}

	filter "system:windows"
		systemversion "latest"
		sysincludedirs { "C:/local/include/*", "C:/*" }
		qtpath "C:/Qt/5.12.0/msvc2017_64"
		links { "ws2_32" }
		targetname "IPChanger"

	filter "system:not windows"
		links { "stdc++fs" }
		qtbinpath "/usr/lib/qt5/bin"
		qtlibpath "/usr/lib/x86_64-linux-gnu"
		qtincludepath "/usr/include/x86_64-linux-gnu/qt5"
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
		(dir .. "/src"),
		(dir .. "/src/**"),
		"ipchanger/src/",
		"ipchanger/src/**"
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