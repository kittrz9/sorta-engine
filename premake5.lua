workspace "openGL-test"
	configurations { "Debug", "Release" }
	platforms { "Linux", "Windows" }
	warnings "Extra"
	-- defining this here so it's defined in every fine without including some header just for this
	defines { "UNUSED=__attribute((unused))" }

	filter { "platforms:Windows" }
		system "windows"
	filter { "platforms:Windows" }
		system "linux"
	filter{}

project "openGL-test"
	kind "ConsoleApp"
	language "C"
	targetdir "build/%{cfg.buildcfg}"

	files { "src/**.h", "src/**.c" }

	-- probably unnecessary to have it specify the architecture but idk maybe I want to make this compile for arm at some point
	filter { "platforms:Windows" }
		architecture "x86_64"
		links { "glew32", "glu32", "opengl32", "glfw3", "portaudio", "m"}
	
	filter { "platforms:Linux" }
		architecture "x86_64"
		links { "GLEW", "EGL", "GL", "GLU", "OpenGL", "glfw", "portaudio", "m"}
	
	filter {}

	includedirs { "src/engine/resourceLoaders/", "src/engine/", "src/game/", "src/game/gameStates"}

	defines { "EXECUTABLE_NAME=\"openGL-test\"" }

	filter "configurations:Debug"
		symbols "On"
	
	filter "configurations:Release"
		optimize "Speed"
