workspace "openGL-test"
	configurations { "Debug", "Release" }
	platforms { "Windows", "Linux" }

project "openGL-test"
	kind "ConsoleApp"
	language "C"
	targetdir "build/%{cfg.buildcfg}"

	files { "src/**.h", "src/**.c" }

	filter { "platforms:Windows" }
		architecture "x86_64"
		defines { "WINDOWS" }
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
		optimize "On"
