workspace "DoomEngine"
	architecture "x64"

	configurations{
		"Debug",
		"Release"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

include "vendor/ImGui"

project "Doom"
	location "Doom"
	kind "SharedLib"
	language "C++"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files {
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs{
		"$(SolutionDir)%{prj.name}/Includes/GLEW",
		"$(SolutionDir)%{prj.name}/Includes/GLFW",
		"$(SolutionDir)vendor"
	}

	libdirs {"$(SolutionDir)%{prj.name}/Libs"}

	links{
		"ImGui",
		"glfw3.lib",
		"opengl32.lib",
		"glew32s.lib",
		"ImGui.lib"
	}

	filter "system:windows"
		systemversion "latest"
		cppdialect "C++17"

	defines{
		"GLEW_STATIC",
		"DOOM_ENGINE",
		"_WIN64"
	}

	postbuildcommands{
		("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outputdir .."/SandBox")
	}

	filter "configurations:Debug"
		defines "_DEBUG"
		symbols "on"

	filter "configurations:Release"
		defines "_RELEASE"
		symbols "on"

project "SandBox"
	location "SandBox"
	kind "ConsoleApp"
	language "C++"

filter "system:windows"
		systemversion "latest"
		cppdialect "C++17"


	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files {
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	libdirs {"$(SolutionDir)Doom/Libs"}

	includedirs{
		"$(SolutionDir)Doom/Includes/GLEW",
		"$(SolutionDir)Doom/Includes/GLFW",
		"$(SolutionDir)Doom/src/Doom",
		"$(SolutionDir)vendor"
	}

	links{
		"Doom",
		"ImGui",
		"opengl32.lib",
		"glfw3.lib",
		"glew32s.lib",
		"ImGui.lib"
	}

	defines{
		"_WIN64",
		"GLEW_STATIC"
	}

	filter "configurations:Debug"
		defines "_DEBUG"
		symbols "on"

	filter "configurations:Release"
		defines "_RELEASE"
		symbols "on"