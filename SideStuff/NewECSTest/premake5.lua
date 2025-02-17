
dirs = {}
dirs["Root"]				= os.realpath("/")
dirs["Bin"]					= os.realpath("Bin/")
dirs["Local"]				= os.realpath("Local/")
dirs["Source"]				= os.realpath("Source/")
dirs["Temp"]				= os.realpath("Temp/")
dirs["Dependencies"]		= os.realpath("Dependencies/")
dirs["Lib"]					= os.realpath("Dependencies/Lib/")

dirs["Launcher"]			= dirs.Source .. "Launcher/"

os.mkdir(dirs.Bin)
os.mkdir(dirs.Local)
os.mkdir(dirs.Source)
os.mkdir(dirs.Temp)
os.mkdir(dirs.Dependencies)
os.mkdir(dirs.Lib)

os.mkdir(dirs.Launcher)

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
		defines { "_SIMPLE", "NDEBUG" }
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

	project "Launcher"
		kind "ConsoleApp"
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
			dirs.Launcher,
		}

		externalincludedirs {

		}

		links {
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