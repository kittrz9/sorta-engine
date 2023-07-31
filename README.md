# sorta engine?
This is a small 2D game engine (I'm still not sure what exactly classifies things as game engines or not but I think this counts) using OpenGL<br>

The main goal of this project is mostly to make a simple thing for me to make other dumb graphical things with (that I'll probably never actually release lmao)<br>

I probably wont implement things like a physics engine or even like a whole thing for collision detection because the main idea with this is to provide as little of the actual "game" part so things can be made to fit the specific game being made (e.g. not having an extremely accurate and complex collision detection when all you really need is AABB collision detection or whatever) <br>

To make stuff you should be able to just edit stuff in the `src/game/` directory to make things.<br>

This is based off of the other thing I made that I'm not sure classifies as a game engine or not (<https://github.com/kittrz9/SDL_ProjectBase>)<br>

As of now this only has support for Linux, this *can* be built for windows but it is currently in a buggy state.<br>

This is pretty much just a project I'm doing for fun and to learn, and I doubt anyone actually would want to use this, though if for some strange reason you want to I'm fine with it. All I'd really want is just to be credited but I'm fine as long as you don't claim this stuff as your own. This project uses the Unlicense so whatever, I don't really care that much.<br>

If you want to contribute to this project I'd appreciate bug fixes but I don't really want big changes that would affect things like the whole structure of the program.<br>

---
## Dependencies
GLEW : <http://glew.sourceforge.net/><br>
GLFW : <https://www.glfw.org/><br>
PortAudio : <http://portaudio.com/><br>
Premake (optional): <https://premake.github.io/><br>

---

## Compiling
### Linux
All you should need to compile this on Linux is glew, glfw3, PortAudio, and premake.<br>
If you're on linux you should be able to get glew, glfw3, portaudio and premake from whatever package manager you're using. <br>
Then all you'd need to do to compile this is run `premake5 gmake` and then `make config=release_linux` and it should compile into the `build/` directory.<br>
<br>
You also have the option to compile with `./build.sh` if you do not want to use Premake.<br>
<br>
### Windows
Windows support is still very buggy and should not be expected to work. To build on windows you need to install [MSYS2](https://www.msys2.org) and then install these packages: `mingw-w64-x86_64-gcc` `mingw-w64-x86_64-glew` `mingw-w64-x86_64-glfw` `mingw-w64-x86_64-portaudio` and `mingw-w64-x86_64-premake`. Then you should be able to just follow the same commands for compiling on Linux, but with the make command being `make config=release_windows`.<br>
As I said, it is still VERY buggy and slow (though I have only been able to test in a virtual machine so that might be why it's slow) so don't expect it to work.<br>
Another thing you'll want to do if you want to run it through the file browser in windows is to put need the files `glew32.dll`, `glfw3.dll`, `libportaudio.dll` and `libwinpthread-1.dll` in the same folder as the executable. I'd recommend getting the first 2 dlls from the sites for each library and get `libportaudio.dll` and `libwinpthread-1.dll` from `C:\msys64\mingw64\bin\`.<br>
However the best solution is probably just to [**get a better operating system.**](https://www.debian.org)<br>


---

## Basic Structure
### src/engine/ directory
This is where the stuff that doesn't really need to be changed when making a game are (though you might actually still have to edit it at some point because of what a mess this piece of shit is lmao)<br>
You will need to include the stuff in the directory though in the `src/game` directory to use them.<br>

#### animation.h and animation.c
Very basic animation handling things using spritesheets.<br>

#### audio.h and audio.c
A very basic audio synthesizer using PortAudio. You provide the ADSR envelope thing and pointer to the function used for the synthesizer in a synthInstrument structure, and then put that with the start and end frequencies, length and volume into a synthData structure and pass that to the playSynth function. It'd probably be better to just read the header and C files to understand how to use it since I can't really explain this well (though it would probably be best to read through all the files that you're going to be using).<br>
You can also load samples using `loadWav` and play it using `playSample` too.<br>

#### entity.h and entity.c
Basically anything that gets updated and/or drawn on screen every frame is an entity. It has a pointer to another struct that has other properties (for sort of polymorphism), 2 function pointers that point to functions that will update and draw the entity that gets called every frame, and a function pointer to destroy any resources being used by the entity when the entity is removed. You can change these function pointers at any time to create a sort of finite state machine (for example you could have it switch from a function to move on the ground to a function to move in the air when jumping)<br>
Entities also have a list of properties for other things to access easier (such as position), the list has property ids and pointers to where the data for the property is stored.<br>

#### files.h and files.c
Some very basic file handling for things that aren't considered resources, along with functions to read a gzip or bzip2 compressed file.<br>

#### logging.h and logging.c
Lets you output to both the terminal and a log file with debug information.<br>

#### renderer.h and renderer.c
Has very basic functions for drawing rectangles and textures to the screen. I should add more functions like them but that's all there is for now.<br>

#### resourceManager.h and resourceManager.c
This lets you load resources. If you try to load a resource you've already loaded it will return the pointer to the already loaded resource.<br>

#### stb_image.h
Not written by me. It's a public domain image library gotten from here: <https://github.com/nothings/stb/blob/master/stb_image.h><br>
It's here so I can load textures easily.<br>

#### vectors.h and vectors.c
Just defines a bunch of vector types and has a few functions for doing stuff with 2d vectors.<br>

#### main.c
The entry point of the program. All that's done here is just initializing a bunch of stuff.<br>

#### src/engine/resourceLoaders/ directory
Where every resource loading and destroying function is defined, you have to update the arrays in `resourceManager.h` and `resourceManager.c`. Currently has support for loading textures and shaders.<br>

<br>

### src/game/ directory
This is where the stuff for the actual game part should be, so stuff like defining game states or whatever<br>

#### controls.h and controls.c
Has a very basic sort of system for handling key presses without having to check with like GLFW if a key is pressed.<br>
You need to change the enum in `controls.h` to list all the controls you need and change the array in `controls.c` to change how each key is defined. The array has to be in the same order as the enum.<br>

#### gameLoop.h and gameLoop.c
Has a while loop that just calls the function in the current game state, updates the delta time and updates the controls. Feels like it should be in the like `src/engine/` directory since it's not really needed to be changed that much for the game but it calls the initialize function of a game state so if you need to change what game state the game starts in you'd have to change it in this file.<br>

#### gameStates.h and gameStates.c
Defines the gameState type. The `initState` and `uninitState` function pointers point to functions that initialize and uninitialize a state, and the `stateLoop` function pointer points to the function that will be called every frame if that state is the current one. Also could probably be moved to the `src/engine/` directory.<br>

#### player.h and player.c
A very basic example of an entity.<br>


### src/game/gameStates/ directory
Where all the game states should be defined. So far only has one defined.

### res/ directory
Where the resources for the game should be.

### res/shaders/ directory
Where the shaders should be.

### res/fonts/ directory
Where the font files should be. They are generated with [msdf-atlas-gen](https://github.com/Chlumsky/msdf-atlas-gen/). Fonts require both a png file and csv file (both generated with msdf-atlas-gen).

---

## Major things to do (in no particular order):

 - Make it not need to start a whole new batch when switching textures
 - Completely rewrite the audio system
 - Music playback (probably using amiga mod files since [libopenmpt](https://lib.openmpt.org/libopenmpt/) is a thing)
 - Fix some of the older rendering functions
 - [ciumgui](https://github.com/cimgui/cimgui) integration (requires cmake though)
 - Optimize stuff (the framerate goes down to about 130fps when there's only like 10000 quads of the same texture on screen)
 - Maybe add in 3D stuff
 - Controller support
 - Make the README file not terrible
 - Support for multiple rendering (and maybe audio) backends like vulkan
 - Reduce memory usage
 - Make log file get put into specific location instead of the current directory
 - Make animations be loaded in from a file
 - Add pitch and time stretching for samples (requires weird calculus stuff I don't understand lmao)
 - Fix the weird high pitched frequencies with resampled samples (happens for both playing back at a different sample rate or loading one with a different sample rate)
 - Fix wavy font breaking on windows
 - Move to GLAD instead of GLEW
 - Maybe start compiling GLAD/GLEW, Portaudio and GLFW into the project to not need to dynamically link with other libraries (would probably make it easier to compile on windows)
