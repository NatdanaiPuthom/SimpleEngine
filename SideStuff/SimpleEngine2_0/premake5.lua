
local directories = {}
directories["Root"]					= os.realpath("\\")
directories["Bin"]					= os.realpath("Bin\\")
directories["Local"]				= os.realpath("Local\\")
directories["Source"]				= os.realpath("Source\\")
directories["Temp"]					= os.realpath("Temp\\")
directories["Dependencies"]			= os.realpath("Dependencies\\")

directories["Lib"]					= directories.Dependencies .. "Lib\\"

directories["Launcher"]				= directories.Source .. "Launcher\\"
directories["External"]				= directories.Source .. "External\\"
directories["Utility"]				= directories.Source .. "Utility\\"
directories["Engine"]				= directories.Source .. "Engine\\"

for key, path in pairs(directories) do
    if key ~= "Root" then 
        os.mkdir(directories[key])
    end
end

workspace "SimpleEngine"
	startproject "Launcher" 
	architecture "x64"
	language "C++"
	cdialect "C17"
	cppdialect "C++20"
	warnings "Extra"
	toolset "msc-v143"
	objdir (directories["Temp"] .. "/%{cfg.buildcfg}/%{prj.name}")
	libdirs (directories["Lib"] .. "/%{cfg.buildcfg}")

	configurations {
		"Debug",
		"Release",
		"Retail"
	}

	filter "configurations:Debug" 
		defines { "_DEBUG" }
		symbols "On"
		optimize "Off"
		runtime "Debug"
		linktimeoptimization "Off"
		editandcontinue "On"		

	filter "configurations:Release"
		defines { "_RELEASE" }
		symbols "On"
		optimize "Speed"
		runtime "Release"
		linktimeoptimization "On"
		editandcontinue "On"

	filter "configurations:Retail"
		defines { "_RETAIL", "NDEBUG" }
		symbols "Off"
		optimize "Full"
		runtime "Release"
		linktimeoptimization "On"
		editandcontinue "Off"

	filter "system:windows"
		systemversion "latest"
		
		flags {
			"MultiProcessorCompile"
		}

	--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	project "External"
		kind "StaticLib"
		location (directories.Local) 
		targetdir (directories["Lib"] .. "/%{cfg.buildcfg}")
		targetname("%{prj.name}_%{cfg.buildcfg}") 
		fatalwarnings { "All" }

		includedirs {
			directories.External, 
		} 

		files {
			directories.External .. "/**.h",
			directories.External .. "/**.hpp",
			directories.External .. "/**.cpp"
		} 
  
	--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	project "Utility"
		kind "StaticLib"
		location (directories.Local) 
		targetdir (directories["Lib"] .. "/%{cfg.buildcfg}")
		targetname("%{prj.name}_%{cfg.buildcfg}") 
		fatalwarnings { "All" }

		includedirs {
			directories.Utility, 
		} 

		files {
			directories.Utility .. "/**.h",
			directories.Utility .. "/**.hpp",
			directories.Utility .. "/**.cpp"
		} 
			
	--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	project "Engine"
		kind "StaticLib"
		location (directories.Local) 
		targetdir (directories["Lib"] .. "/%{cfg.buildcfg}")
		targetname("%{prj.name}_%{cfg.buildcfg}") 
		fatalwarnings { "All" }

		includedirs {
			directories.Source,
			directories.Engine
		} 

		files {
			directories.Engine .. "/**.h",
			directories.Engine .. "/**.hpp",
			directories.Engine .. "/**.cpp"
		} 

		links {
			"Utility"
		}
			
	--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	project "Launcher"
		kind "WindowedApp"
		location (directories.Local)
		targetdir (directories["Bin"] .. "/%{cfg.buildcfg}")
		targetname "SimpleEngine_%{cfg.buildcfg}"
		fatalwarnings { "All" }
	
		files {
			directories.Launcher .. "/**.h",
			directories.Launcher .. "/**.hpp",
			directories.Launcher .. "/**.cpp",
			directories.Launcher .. "/**.rc"
		}

		includedirs { 
			directories.Source,
			directories.Launcher
		}

		links {
			"External", 
			"Engine"
		}

		filter "system:windows"
			systemversion "latest"

			links {
				"dwmapi"
			}

	--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------