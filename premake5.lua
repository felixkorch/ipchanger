require( "premake-qt/qt" )
local qt = premake.extensions.qt

workspace "IPChanger"
	architecture "x64"
	startproject "IPChanger-GUI"

	configurations
	{
		"Debug",
		"Release"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

project "IPChanger"
	location "ipchanger"
	kind "StaticLib"
	staticruntime "Off"
	language "C++"
	cppdialect "C++17"

	pchheader "pch.h"
	pchsource "ipchanger/src/pch.cpp"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/src/**.h"
	}

	includedirs
	{
		"%{prj.name}/src",
		"%{prj.name}/src/**"
	}

	filter "system:windows"
		systemversion "latest"
		sysincludedirs { "C:/local/include/*" }
		--postbuildcommands
		--{
		--	("{COPY} %{cfg.buildtarget.relpath} \"../bin/" .. outputdir .. "/IPChanger-GUI/\"")
		--}
		links { "ws2_32" }

	filter "system:linux"
		links { "stdc++fs" }

	filter "system:macosx"
		links { "stdc++fs" }

	filter "configurations:Debug"
		defines { "DEBUG" }
		symbols "On"

	filter "configurations:Release"
		defines { "NDEBUG" }
		optimize "On"


project "IPChanger-GUI"
	location "gui"
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
		"gui/src/**.cpp",
		"gui/src/**.h",
		"gui/src/**.ui",
		"gui/src/**.qrc"
	}

	includedirs
	{
		"gui/src",
		"gui/src/**",
		"ipchanger/src/",
		"ipchanger/src/**"
	}

	links
	{
		"IPChanger"
	}

	filter "system:windows"
		systemversion "latest"
		sysincludedirs { "C:/local/include/*" }
		qtpath "C:/Qt/5.12.0/msvc2017_64"
		links { "ws2_32" }

	filter "system:linux"
		links { "stdc++fs" }

	filter "system:macosx"
		links { "stdc++fs" }

	filter "configurations:Debug"
		defines { "DEBUG" }
		symbols "On"

	filter "configurations:Release"
		defines { "NDEBUG" }
		runtime "Release"
		optimize "On"