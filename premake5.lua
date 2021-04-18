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
		"%{prj.name}/**.h",
		"%{prj.name}/**.cpp"
	}

	includedirs{
		"$(SolutionDir)Includes/GLEW",
		"$(SolutionDir)Includes/GLFW",
		"$(SolutionDir)Includes/LUA",
		"$(SolutionDir)vendor",
		"$(SolutionDir)%{prj.name}/src/%{prj.name}",
		
	}

	libdirs {"$(SolutionDir)Libs"}

	links{
		"glfw3.lib",
		"opengl32.lib",
		"glew32s.lib",
		"ImGui.lib",
		"zlib.lib",
		"OpenAL32.lib",
		"ImGUi",
	}

	filter "system:windows"
		systemversion "latest"
		cppdialect "C++17"

	defines{
		"_CRT_SECURE_NO_WARNINGS",
		"GLEW_STATIC",
		"DOOM_ENGINE",
		"_WIN64",
		"VORBIS_FPU_CONTROL"
	}

	postbuildcommands{
		("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outputdir .. "/SandBox"),
	}

	flags {
	 "MultiProcessorCompile",
	 }

	filter "configurations:Debug"
		runtime "Debug"
		defines "_DEBUG"
		symbols "on"

	filter "configurations:Release"
		defines "_RELEASE"
		runtime "Release"
		symbols "on"
		optimize "Full"

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
		"%{prj.name}/**.h",
		"%{prj.name}/**.cpp"
	}

	libdirs {
		"$(SolutionDir)Libs",
		"bin/" .. outputdir .. "/Doom"
	}

	includedirs{
		"$(SolutionDir)Includes/GLEW",
		"$(SolutionDir)Includes/GLFW",
		"$(SolutionDir)SandBox",
		"$(SolutionDir)vendor",
		"$(SolutionDir)%{prj.name}/src",
		"$(SolutionDir)Doom/src/Doom",
		"$(SolutionDir)Includes/LUA",
	}

	flags {
	 "MultiProcessorCompile",
	 }

	links{
		"glfw3.lib",
		"opengl32.lib",
		"glew32s.lib",
		"ImGui.lib",
		"zlib.lib",
		"OpenAL32.lib",
		"Doom.lib",
		"Doom",
		"ImGUi",
	}

	defines{
		"_CRT_SECURE_NO_WARNINGS",
		"_WIN64",
		"GLEW_STATIC",
		"_WINDLL"
	}

	filter "configurations:Debug"
		defines "_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "_RELEASE"
		runtime "Release"
		symbols "on"
		optimize "Full"