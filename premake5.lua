
workspace "SimpleEngine"
	startproject "Game"
	architecture "x64"
	language "C++"
	cppdialect "C++20"
	warnings "Extra" -- Highest Warning Level
	flags { "FatalWarnings" } -- Threat All Warnings As Errors
	objdir "Temp" -- Location for garbage created by Visual Studio

	local shader_dir = path.join(_SCRIPT_DIR, "Bin/Shaders/") -- Absolute path to this file plus "Bin/Shaders/"
	os.mkdir(shader_dir)

	local profiler_dir = path.join(_SCRIPT_DIR, "Bin/Profilers/")
	os.mkdir(profiler_dir)

	defines { -- Create Global Macro For This "Path"
		'SIMPLE_SHADER_DIR="' .. shader_dir .. '/"' ,
		'SIMPLE_PROFILER_DIR="' ..profiler_dir ..'/"'
	}

	configurations {
		"Debug",
		"Release",
		"Simple"
	}

	filter "configurations:Debug"
		defines { "_DEBUG" }
		symbols "On"
		runtime "Debug"

	filter "configurations:Release"
		defines { "_RELEASE" }
		optimize "On"
		runtime "Release"

	filter "configurations:Simple"
		defines { "_SIMPLE" }
		optimize "Speed"
		runtime "Release"

	project "Engine"
		kind "StaticLib"
		targetdir "Lib"
		targetname("%{prj.name}_%{cfg.buildcfg}")
		location "Source/Engine"
		includedirs{"Source/" , "Source/Engine/", "Source/Engine/Graphics/", "Source/External/"} -- Included "Source/" so that we do not need "../../Engine/Graphics/Test.h" and instead can do "Engine/Graphics/Test.h"
		files {"Source/Engine/**.h", "Source/Engine/**.cpp","Source/Engine/Graphics/Shaders/**.hlsl" , "Source/Engine/Graphics/Shaders/**.hlsli"} -- Files that shown in Visual Studio's Solution
		dependson { "External" }
		links {"External", "d3d11"}

		pchheader "stdafx.h" -- Precomplier header to reduce common files from begin unnessesary complied multiple times
		pchsource "Source/Engine/stdafx.cpp"

		shadermodel("5.0")
		shaderoptions({"/WX"})
		
		filter("files:**.hlsl")
			 local output_dir = path.join(shader_dir, "%{file.basename}.cso")
				shaderobjectfileoutput(output_dir) -- Create .cso files at this location

		filter("files:Source/Engine/Graphics/Shaders/PS/**.hlsl")
			shadertype("Pixel") -- Set all .hlsl shadertype in this folder to "Pixel"

		filter("files:Source/Engine/Graphics/Shaders/VS/**.hlsl")
			shadertype("Vertex")  -- Set all .hlsl shadertype in this folder to "Vertex"

	project "External"
		kind "StaticLib"
		targetdir "Lib"
		targetname("%{prj.name}_%{cfg.buildcfg}")
		location "Source/External"
		includedirs{ "Source/External/" }
		files {"Source/External/**.h", "Source/External/**.cpp"}

	project "Game"
		kind "WindowedApp"
		targetdir "Bin"
		targetname "SimpleEngine_%{cfg.buildcfg}"
		location "Source/Game"
		includedirs{ "Source/", "Source/Engine/" ,"Source/Game/", "Source/External/"}
		files {"Source/Game/**.h", "Source/Game/**.cpp", "Source/Game/Resources/**.rc"}
		dependson { "Engine" }
		libdirs{"Lib", "DLL"}
		links { "Engine", "easy_profiler.lib", "easy_profiler.dll"}

        postbuildcommands { -- Copy DLL to Bin
            "{COPY} %{wks.location}/DLL/easy_profiler.dll %{cfg.targetdir}"
        }
