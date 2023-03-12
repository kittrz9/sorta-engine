workspace "openGL-test"
	configurations { "Debug", "Release" }

project "openGL-test"
	kind "ConsoleApp"
	language "C"
	targetdir "build/%{cfg.buildcfg}"

	files { "src/**.h", "src/**.c" }

	links { "GLEW", "EGL", "GL", "GLU", "OpenGL", "glfw", "portaudio", "m"}

	includedirs { "src/engine/resourceLoaders/", "src/engine/", "src/game/", "src/game/gameStates"}

	defines { "EXECUTABLE_NAME=\"openGL-test\"" }

	filter "configurations:Debug"
		symbols "On"
	
	filter "configurations:Release"
		optimize "On"
