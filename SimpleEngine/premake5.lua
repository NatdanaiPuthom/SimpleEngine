
-- In future if this file gets too big I may split them into multiple premake in each project. For now they are very small and follow a pattern which is easier to follow for beginner

workspace "SimpleEngine" -- Workspace, is not same as Project. Anything configurated here is in Global scope (affects all projects)

	startproject "Launcher" -- Start up Project
	architecture "x64" -- 64-bit architecture
	language "C++" -- Global language across all Projects in this workspace
	cppdialect "C++20" -- Global C++ Version
	warnings "Extra" -- Global Highest Warning Level
	objdir "Temp" -- Location for garbage created by Visual Studio

	local bin_dir = path.join(_SCRIPT_DIR, "Bin/") -- Returns absolute path to where this premake file is plus "Bin/" example -> C:\Users\panda\Desktop\SimpleEngine\Bin\
	local shader_dir = path.join(_SCRIPT_DIR, "Bin/Shaders/") 
	local profiler_dir = path.join(_SCRIPT_DIR, "Bin/Profilers/")
	local settings_dir = path.join(_SCRIPT_DIR, "Bin/Settings/")

	os.mkdir(bin_dir)  -- Create these folders on call "generate_project.bat" (current .bat file name)
	os.mkdir(shader_dir)
	os.mkdir(profiler_dir)
	os.mkdir(settings_dir)
	
	dirs = {} -- Absolute path to specific folder
	dirs["root"]           = os.realpath("/")
	dirs["Bin"]            = os.realpath("Bin/")
	dirs["Local"]          = os.realpath("Local/")
	dirs["Dependencies"]   = os.realpath("Dependencies/")
	dirs["Lib"]            = os.realpath("Dependencies/Lib/")
	dirs["DLL"]            = os.realpath("Dependencies/DLL/")
	dirs["SimpleLib"]      = os.realpath("Dependencies/SimpleLib/")

	defines { -- Create Global Macro For Strings
		'SIMPLE_IMGUI_FILENAME="' .."Settings/imgui.ini" .. '"', -- In Visual Studio "SIMPLE_IMGUI_FILENAME" will be equal to const char* "imgui.ini"
		'SIMPLE_SETTINGS_FILENAME="' .. "Settings/settings.json" .. '"',
		'SIMPLE_PROFILER_FILENAME="' .. "Profilers/profiler_data.prof" .. '"',
		'SIMPLE_LEVELS_FILENAME="' .. "Settings/levels.json" .. '"',
		'SIMPLE_TEXTURES_DIR="' .. "Assets/Textures/" .. '"',
		'SIMPLE_SHADERS_DIR="' .. "Shaders/" .. '"',
		'SIMPLE_NAVMESH_DIR="' .. "Assets/Navmesh/" .. '"'
	}

	configurations { -- Create Configurations For This WorkSpace
		"Debug",
		"Release",
		"Simple"
	}

	filter "configurations:Debug" -- Global Configurations Settings
		defines { "_DEBUG" }
		symbols "On"
		optimize "Off"
		runtime "Debug"

	filter "configurations:Release"
		defines { "_RELEASE" }
		optimize "Speed"
		runtime "Release"

	filter "configurations:Simple"
		defines { "_SIMPLE" }
		symbols "Off"
		optimize "Full"
		runtime "Release"

	filter "system:windows"
		systemversion "latest"
  
	--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	project "External"
		kind "StaticLib"
		location  (dirs.Local)
		targetdir (dirs.SimpleLib)
		targetname("%{prj.name}_%{cfg.buildcfg}")
		flags { "FatalCompileWarnings" } -- Treat ComplieWarnings as error but not Lib/Link warnings as error

		defines {
			"FBXSDK_SHARED" 
		}

		includedirs {
			"Source/",
			"Source/External/", 
			"Source/External/dearimgui",
			"Source/External/dearimgui/imgui/", 
			"Source/External/dearimgui/freetype/",
			"Source/External/TheGameAssembly/",
			"Source/External/TheGameAssembly/FBXImporter/",
			"Source/External/TheGameAssembly/FBXImporter/Source",
			"Source/External/TheGameAssembly/FBXImporter/FBXSDK/"
		}

		files {
			"Source/External/**.h", 
			"Source/External/**.cpp",
			"Source/External/**.hpp" 
		}

		libdirs { 
				dirs.Lib
			}

		links {
			"Lib/Common/easy_profiler"
			}

		filter "configurations:Debug" 
			links { 
				"Lib/Debug/libfbxsdk"	
			}
			
		filter "configurations:Release" 
			links { 
				"Lib/Release/libfbxsdk"
			}

		filter "configurations:Simple" 
			links { 
				"Lib/Release/libfbxsdk"
			}	

	--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	project "Engine"
		kind "StaticLib" -- Build type ".lib"
		location (dirs.Local) -- I only know that this is for where vcxproj, vcxproj.filter and vcxproj.user will be generated
		targetdir (dirs.SimpleLib) -- Location of where the build will be. Example -> C:\Users\panda\Desktop\SimpleEngine\Lib
		targetname("%{prj.name}_%{cfg.buildcfg}") -- Name of the Build plus Active Configuration type. Example -> "Engine_Debug"
		-- dependson { "External" } -- Ensure that "External" is built before this project
		flags { "FatalWarnings" }

		pchheader "Engine/Precomplied/EnginePch.hpp" -- Force all Source (.cpp) files in this project to include "Engine/Precomplier/stdafx.h"
		pchsource "Source/Engine/Precomplied/EnginePch.cpp" -- Telling this project to use precomplier header

		includedirs { -- Included "Source/" so that we do not need "../../Engine/Graphics/Test.h" and instead can do "Engine/Graphics/Test.h"
			"Source/" ,
			"Source/Engine/", 
			"Source/Engine/Graphics/", 
			"Source/External/"
		} 

			files { -- Files that shown in Visual Studio's Solution
			"Source/Engine/**.h", 
			"Source/Engine/**.hpp", 
			"Source/Engine/**.cpp"
		} 

		libdirs {
			dirs.Lib
		}

		links { -- Linking these Libraries during build
			--"External"
		}
			
	--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	project "Graphics"
		kind "StaticLib"
		location (dirs.Local)
		targetdir (dirs.SimpleLib)
		targetname("%{prj.name}_%{cfg.buildcfg}") 
		-- dependson { "Engine" }
		flags { "FatalWarnings" }

		pchheader "Graphics/Precomplied/GraphicsPch.hpp" -- Force all Source (.cpp) files in this project to include "Engine/Precomplier/stdafx.h"
		pchsource "Source/Graphics/Precomplied/GraphicsPch.cpp" -- Telling this project to use precomplier header

		files {
			"Source/Graphics/**.h",
			"Source/Graphics/**.hpp",
			"Source/Graphics/**.cpp",
			"Source/Graphics/**.hlsl",
			"Source/Graphics/**.hlsli"
		}

		includedirs {
			"Source/",
			"Source/Engine/",
			"Source/External/",
			"Source/Graphics/"
		}

		links {
			--"Engine",
			"d3d11"
		}

		shadermodel("5.0")
		shaderoptions({"/WX"})

		filter("files:**.hlsl")
			local output_dir = path.join(shader_dir, "%{file.basename}.cso") -- Absolute path and name of the files. Example -> C:\Users\panda\Desktop\SimpleEngine\Bin\Shaders\DefaultPS.cso
			shaderobjectfileoutput(output_dir) -- Create .cso files at this location

		filter("files:Source/Graphics/Shaders/PS/**.hlsl")
			shadertype("Pixel") -- Set all .hlsl shadertype in this folder to "Pixel"

		filter("files:Source/Graphics/Shaders/VS/**.hlsl")
			shadertype("Vertex")  -- Set all .hlsl shadertype in this folder to "Vertex"
	

	--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	project "Game"
		kind "StaticLib"
		location (dirs.Local)
		targetdir (dirs.SimpleLib)
		targetname("%{prj.name}_%{cfg.buildcfg}") 
		-- dependson { "Graphics" }
		flags { "FatalWarnings" }

		pchheader "Game/Precomplied/GamePch.hpp"
		pchsource "Source/Game/Precomplied/GamePch.cpp"

		files {
			"Source/Game/**.h", 
			"Source/Game/**.hpp", 
			"Source/Game/**.cpp"
		}

		includedirs { 
			"Source/",
			"Source/Engine/" ,
			"Source/Game/",
			"Source/External/"
		}

		libdirs {
			dirs.Lib
		}

		links {
			--"Graphics"
		}

	--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	project "Launcher"
		kind "WindowedApp"
		location (dirs.Local)
		targetdir (dirs.Bin)
		targetname "SimpleEngine_%{cfg.buildcfg}"
		flags { "FatalWarnings" }

		--dependson {
		--	"External", 
		--	"Engine", 
		--	"GraphicsEngine",
		--	"Game"  
		--}
	
		files {
			"Source/Launcher/**.h", 
			"Source/Launcher/**.hpp", 
			"Source/Launcher/**.cpp", 
			"Source/Launcher/**.rc"
		}

		includedirs { 
			"Source/",
			"Source/Engine/",
			"Source/Graphics/",
			"Source/Game/",
			"Source/External/"
		}

		externalincludedirs {
			"Source/External/"
		}

		libdirs {
			dirs.Lib
		}

		links {
			"External", 
			"Engine", 
			"Graphics", 
			"Game"
		}

		postbuildcommands {
			"{COPY} %{wks.location}/Dependencies/Settings/* " .. settings_dir,
			"{COPY} %{wks.location}/Dependencies/DLL/Common/*.dll %{cfg.targetdir}"
		}

		filter "configurations:Debug"
			postbuildcommands {
				"{COPY} %{wks.location}/Dependencies/DLL/Debug/*.dll %{cfg.targetdir}"
			}

		filter "configurations:Release"
			postbuildcommands {
				"{COPY} %{wks.location}/Dependencies/DLL/Release/*.dll %{cfg.targetdir}"
			}

		filter "configurations:Simple"
			postbuildcommands {
				"{COPY} %{wks.location}/Dependencies/DLL/Release/*.dll %{cfg.targetdir}"
			}