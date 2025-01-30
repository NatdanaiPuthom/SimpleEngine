
dirs = {}
dirs["Root"]				= os.realpath("/")
dirs["Bin"]					= os.realpath("Bin/")
dirs["Local"]				= os.realpath("Local/")
dirs["Source"]				= os.realpath("Source/")
dirs["Temp"]				= os.realpath("Temp/")
dirs["Dependencies"]		= os.realpath("Dependencies/")
dirs["Lib"]					= os.realpath("Dependencies/Lib/")

dirs["Launcher"]			= dirs.Source .. "Launcher/"
dirs["Utility"]				= dirs.Source .. "Utility/"
dirs["Engine"]				= dirs.Source .. "Engine/"
dirs["UnitTest"]			= dirs.Source .. "UnitTest/"

os.mkdir(dirs.Bin)
os.mkdir(dirs.Local)
os.mkdir(dirs.Source)
os.mkdir(dirs.Temp)
os.mkdir(dirs.Dependencies)
os.mkdir(dirs.Lib)

os.mkdir(dirs.Launcher)
os.mkdir(dirs.Utility)
os.mkdir(dirs.Engine)
os.mkdir(dirs.UnitTest)

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

	project "Utility"
		kind "StaticLib"
		location (dirs.Local) 
		targetdir (dirs.Lib)
		targetname("%{prj.name}_%{cfg.buildcfg}") 
		flags { "FatalWarnings" }

		includedirs {
			dirs.Utility, 
		} 

		files {
			dirs.Utility .. "/**.h",
			dirs.Utility .. "/**.hpp",
			dirs.Utility .. "/**.cpp",
			dirs.Utility .. "/**.ixx",
		} 
			
	--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	project "Engine"
		kind "StaticLib"
		location (dirs.Local) 
		targetdir (dirs.Lib)
		targetname("%{prj.name}_%{cfg.buildcfg}") 
		flags { "FatalWarnings" }

		includedirs {
			dirs.Engine, 
		} 

		files {
			dirs.Engine .. "/**.h",
			dirs.Engine .. "/**.hpp",
			dirs.Engine .. "/**.cpp",
			dirs.Engine .. "/**.ixx",
		} 

		links {
			"Utility"
		}
			
	--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	project "UnitTest"
		kind "SharedLib"
		location (dirs.Local)
		targetdir (dirs.Temp)
		targetname("%{prj.name}_%{cfg.buildcfg}") 
		flags { "FatalWarnings" }

		files {

			dirs.UnitTest .. "/**.h",
			dirs.UnitTest .. "/**.hpp",
			dirs.UnitTest .. "/**.cpp",
			dirs.UnitTest .. "/**.ixx",
		}

		includedirs {
			dirs.Source,
			dirs.UnitTest
		}

		links {
			"Utility"
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
			dirs.Launcher,
		}

		externalincludedirs {

		}

		links {
			"Engine",
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