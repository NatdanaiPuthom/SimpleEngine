
local directories = {}
directories["Root"]					= os.realpath("\\")
directories["Bin"]					= os.realpath("Bin\\")
directories["Local"]				= os.realpath("Local\\")
directories["Source"]				= os.realpath("Source\\")
directories["Temp"]					= os.realpath("Temp\\")
directories["Dependencies"]			= os.realpath("Dependencies\\")

directories["Lib"]					= directories.Dependencies .. "Lib\\"
directories["Launcher"]				= directories.Source .. "Launcher\\"
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
	warnings "Everything"
	toolset "msc-v143"
	objdir (directories.Temp)

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
		linktimeoptimization "Off"
		editandcontinue "On"

	filter "configurations:Release"
		defines { "_RELEASE" }
		symbols "On"
		optimize "Speed"
		runtime "Release"
		linktimeoptimization "On"
		editandcontinue "On"

	filter "configurations:Simple"
		defines { "_SIMPLE", "NDEBUG" }
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

		links {
			"dwmapi"
		}
  
	--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	project "Engine"
		kind "StaticLib"
		location (directories.Local) 
		targetdir (directories.Lib)
		targetname("%{prj.name}_%{cfg.buildcfg}") 
		fatalwarnings { "All" }

		includedirs {
			directories.Engine, 
		} 

		files {
			directories.Engine .. "/**.h",
			directories.Engine .. "/**.hpp",
			directories.Engine .. "/**.cpp"
		} 

		links {
		}
			
	--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	project "Launcher"
		kind "WindowedApp"
		location (directories.Local)
		targetname "SimpleEngine_%{cfg.buildcfg}"
		fatalwarnings { "All" }
	
		files {
			directories.Launcher .. "/**.h",
			directories.Launcher .. "/**.hpp",
			directories.Launcher .. "/**.cpp"
		}

		includedirs { 
			directories.Launcher,
		}

		links {
			"Engine"
		}

	--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------