
dirs = {}
dirs["Root"]				= os.realpath("/")
dirs["Bin"]					= os.realpath("Bin/")
dirs["Local"]				= os.realpath("Local/")
dirs["Source"]				= os.realpath("Source/")
dirs["Temp"]				= os.realpath("Temp/")
dirs["Dependencies"]		= os.realpath("Dependencies/")
dirs["Lib"]					= os.realpath("Dependencies/Lib/")

dirs["Launcher"]			= os.realpath("Source/Launcher/")
dirs["Utility"]				= os.realpath("Source/Utility/")
dirs["Engine"]				= os.realpath("Source/Engine/")
dirs["UnitTests"]			= os.realpath("Source/UnitTests/")

os.mkdir(dirs.Bin)
os.mkdir(dirs.Local)
os.mkdir(dirs.Source)
os.mkdir(dirs.Temp)
os.mkdir(dirs.Dependencies)
os.mkdir(dirs.Lib)

os.mkdir(dirs.Launcher)
os.mkdir(dirs.Utility)
os.mkdir(dirs.Engine)
os.mkdir(dirs.UnitTests)

dirs["Test"]				= os.realpath("Bin/Test/")

workspace "SimpleEngine"
	startproject "Launcher" 
	architecture "x64"
	language "C++"
	cdialect "C17"
	cppdialect "C++20"
	warnings "Extra"
	objdir (dirs.Temp)

	defines {
		
	}

	configurations {
		"Debug",
		"Release",
		"Simple"
	}

	filter "configurations:Debug" 
		defines { "_DEBUG" }
		symbols "On"
		optimize "Off"
		runtime "Debug"
		libdirs { dirs.Lib	}

	filter "configurations:Release"
		defines { "_RELEASE" }
		symbols "On"
		optimize "Speed"
		runtime "Release"
		libdirs { dirs.Lib	}

	filter "configurations:Simple"
		defines { "_SIMPLE" }
		symbols "Off"
		optimize "Full"
		runtime "Release"
		libdirs { dirs.Lib	}

	filter "system:windows"
		systemversion "latest"
		
		flags {
			"MultiProcessorCompile"
		}

		links {
			"dwmapi"
		}
  
	--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	project "Utility"
		kind "StaticLib"
		location (dirs.Local) 
		targetdir (dirs.Lib)
		targetname("%{prj.name}_%{cfg.buildcfg}") 
		flags { "FatalWarnings" }

		includedirs {
			dirs.Source,
			dirs.Utility, 
		} 

		files {
			dirs.Utility .. "/**.h",
			dirs.Utility .. "/**.hpp",
			dirs.Utility .. "/**.cpp",
			dirs.Utility .. "/**.ixx",
		} 
			
	--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	
	project "UnitTests"
		kind "SharedLib"
		location (dirs.Local)
		targetdir (dirs.Temp)
		targetname "UnitTests_%{cfg.buildcfg}"

		flags { 
			"FatalWarnings" 
		}

		files {
			"Source/UnitTests/**.h",
			"Source/UnitTests/**.hpp",
			"Source/UnitTests/**.cpp",
		}

		includedirs {
			dirs.Source
		}

		links {
		}

	--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	project "Engine"
		kind "StaticLib"
		location (dirs.Local) 
		targetdir (dirs.Lib)
		targetname("%{prj.name}_%{cfg.buildcfg}") 
		flags { "FatalWarnings" }

		includedirs {
			dirs.Source,
			dirs.Engine, 
		} 

		files {
			dirs.Engine .. "/**.h",
			dirs.Engine .. "/**.hpp",
			dirs.Engine .. "/**.cpp",
			dirs.Engine .. "/**.ixx",
		} 
			
	--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	project "Launcher"
		kind "WindowedApp"
		location (dirs.Local)
		targetname "TestEngine_%{cfg.buildcfg}"
		flags { "FatalWarnings" }
	
		files {
			dirs.Launcher .. "/**.h",
			dirs.Launcher .. "/**.hpp",
			dirs.Launcher .. "/**.cpp",
			dirs.Launcher .. "/**.ixx",
		}

		includedirs { 
			dirs.Source,
			dirs.Launcher,
			dirs.Utility,
		}

		externalincludedirs {

		}

		links {
			"Engine",
			"Utility"
		}

		prebuildcommands {
		}

		filter "configurations:Debug"
			targetdir (dirs.Bin)

			postbuildcommands {
			
			}

		filter "configurations:Release"
			targetdir (dirs.Bin)

			postbuildcommands {
				
			}

		filter "configurations:Simple"
			targetdir (dirs.Bin)

			postbuildcommands {
				
			}