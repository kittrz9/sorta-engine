workspace "openGL-test"
	configurations { "Debug", "Release" }
	platforms { "Linux", "Windows" }
	warnings "Extra"
	-- defining this here so it's defined in every fine without including some header just for this
	defines { "UNUSED=__attribute__((unused))" }

	newoption {
		trigger = "nobzip2",
		description = "Disables bzip2 decompression support"
	}

	newoption {
		trigger = "nogzip",
		description = "Disables gzip decompression support"
	}

	filter { "options:nobzip2" }
		defines { "NO_BZIP2_SUPPORT" }

	filter { "options:nogzip" }
		defines { "NO_GZIP_SUPPORT" }

	filter { "platforms:Windows" }
		system "windows"
	filter { "platforms:Linux" }
		system "linux"
	filter{}

project "openGL-test"
	kind "WindowedApp"
	language "C"
	targetdir "build/%{cfg.buildcfg}"

	targetname ("%{wks.name}")

	files { "src/**.h", "src/**.c" }

	links { "glfw", "portaudio", "m" }

	filter { "options:not nogzip" }
		links { "z" }

	filter { "options:not nobzip22" }
		links { "bz2" }
	
	filter {}

	includedirs { "src/engine/resourceLoaders/", "src/engine/", "src/game/", "src/game/gameStates", "src/external" }

	filter { "configurations:Debug" }
		symbols "On"
	
	filter { "configurations:Release" }
		optimize "Speed"

-- commented out because I probably wont actually do whatever I was thinking about with dynamic linking
-- maybe later I'll try doing weird shenanigans with hot code reloading like in this: https://www.youtube.com/watch?v=Y57ruDOwH1g

--project "stb_image"
--	kind "SharedLib"
--	language "C"
--	targetdir "build/%{cfg.buildcfg}"
--
--	-- moved into a seperate directory so that the engine can just have "src/engine/*.h" instead of manually putting every new file
--	files { "src/engine/stb_image/**.h", "src/engine/stb_image/**.c" }
--
--	filter { "platforms:Windows" }
--		architecture "x86_64"
--
--	filter { "platforms:Linux" }
--		architecture "x86_64"
--	
--	filter {}
--
--	includedirs { "src/engine/stb_image/", "src/engine/" }
--
--	-- probably best to optimize for size since it takes up a ton of space
--	optimize "Size"
--
--project "resourceLoaders"
--	kind "SharedLib"
--	language "C"
--	targetdir "build/%{cfg.buildcfg}"
--
--	files { "src/engine/resourceLoaders/**.h", "src/engine/resourceLoaders/**.c" }
--
--	filter { "platforms:Windows" }
--		architecture "x86_64"
--		links { "glew32", "glu32", "opengl32", "glfw3","stb_image" }
--
--	filter { "platforms:Linux" }
--		architecture "x86_64"
--		links { "GLEW", "EGL", "GL", "GLU", "OpenGL", "glfw", "m", "stb_image"}
--	
--	filter {}
--
--	includedirs { "src/engine/stb_image", "src/engine/resourceLoaders/", "src/engine/" }
--
--	filter { "configurations:Debug" }
--		symbols "On"
--	
--	filter { "configurations:Release" }
--		optimize "Speed"
--
--project "engine"
--	kind "SharedLib"
--	language "C"
--	targetdir "build/%{cfg.buildcfg}"
--
--	files { "src/engine/*.h", "src/engine/*.c" }
--
--	filter { "platforms:Windows" }
--		architecture "x86_64"
--		links { "glew32", "glu32", "opengl32", "glfw3", "portaudio", "m" }
--
--	filter { "platforms:Linux" }
--		architecture "x86_64"
--		links { "GLEW", "EGL", "GL", "GLU", "OpenGL", "glfw", "portaudio", "m" }
--
--	filter { "options:not nogzip" }
--		links { "z" }
--
--	filter { "options:not nobzip22" }
--		links { "bz2" }
--	
--	filter {}
--
--	includedirs { "src/engine/resourceLoaders/", "src/engine/", "src/game/" }
--
--	filter { "configurations:Debug" }
--		symbols "On"
--	
--	filter { "configurations:Release" }
--		optimize "Speed"
--
--project "game"
--	kind "WindowedApp"
--	language "C"
--	targetdir "build/%{cfg.buildcfg}"
--
--	targetname ("%{wks.name}")
--
--	files { "src/game/**.h", "src/game/**.c" }
--
--	filter { "platforms:Windows" }
--		architecture "x86_64"
--		links { "glew32", "glu32", "opengl32", "glfw3", "portaudio", "m", "engine", "resourceLoaders" }
--
--	filter { "platforms:Linux" }
--		architecture "x86_64"
--		links { "GLEW", "EGL", "GL", "GLU", "OpenGL", "glfw", "m", "engine", "resourceLoaders" }
--
--	filter { "options:not nogzip" }
--		links { "z" }
--
--	filter { "options:not nobzip22" }
--		links { "bz2" }
--	
--	filter {}
--
--	includedirs { "src/engine/", "src/game/", "src/game/gameStates" }
--
--	filter { "configurations:Debug" }
--		symbols "On"
--	
--	filter { "configurations:Release" }
--		optimize "Speed"
--
