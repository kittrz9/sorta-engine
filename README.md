# sorta engine?
This is a small 2D game engine (I'm still not sure what exactly classifies things as game engines or not but I think this counts) using OpenGL  

The main goal of this project is mostly to make a simple thing for me to make other dumb graphical things with (that I'll probably never actually release lmao)  

I probably wont implement things like a physics engine or even like a whole thing for collision detection because the main idea with this is to provide as little of the actual "game" part so things can be made to fit the specific game being made (e.g. not having an extremely accurate and complex collision detection when all you really need is AABB collision detection or whatever)   

To make stuff you should be able to just edit stuff in the `src/game/` directory to make things.  

This is based off of the other thing I made that I'm not sure classifies as a game engine or not (<https://github.com/kittrz9/SDL_ProjectBase>)  

As of now this only has support for Linux, this *can* be built for windows but it is currently in a buggy state.  

This is pretty much just a project I'm doing for fun and to learn, and I doubt anyone actually would want to use this, though if for some strange reason you want to I'm fine with it. All I'd really want is just to be credited but I'm fine as long as you don't claim this stuff as your own. This project uses the Unlicense so whatever, I don't really care that much.  

If you want to contribute to this project I'd appreciate bug fixes but I don't really want big changes that would affect things like the whole structure of the program.  

---
## Dependencies
GLAD (already in project) : <https://github.com/Dav1dde/glad/tree/glad2>  
GLFW : <https://www.glfw.org/>  
PortAudio (can be compiled into the project) : <http://portaudio.com/>  

---

## Compiling
### Linux
All you should need to compile this on Linux is glfw3 and PortAudio. (though you can compile PortAudio with the project when using build.sh)  
If you're on linux you should be able to get glfw3 (and PortAudio if you're using it externally) from whatever package manager you're using.   
Then you should just be able to run the included `build.sh` script. **Please read through this script to know what environment variables you can change when running it.**  
Everything should be compiled into the `build/` directory once it is finished.  
  
You also have the option to use premake if you are either having issues with the build script or just prefer premake.  
**This is not recommended however, as I will probably not update it as often as the build script and it will likely be out of date.**  
**If you are using premake, you are expected to edit the file yourself.**  
If you have premake installed, simply run `premake5 gmake` and then `make config=release_linux`  
  
### Windows
Windows support is still very buggy and should not be expected to work. To build on windows you need to install [MSYS2](https://www.msys2.org) and then install these packages: `mingw-w64-x86_64-gcc` `mingw-w64-x86_64-glfw` `mingw-w64-x86_64-portaudio` and `mingw-w64-x86_64-premake`. Then you should be able to just follow the same commands for compiling on Linux, but with the make command being `make config=release_windows`.  
As I said, it is still VERY buggy and slow (though I have only been able to test in a virtual machine so that might be why it's slow) so don't expect it to work.  
Another thing you'll want to do if you want to run it through the file browser in windows is to put need the files `glfw3.dll`, `libportaudio.dll` and `libwinpthread-1.dll` in the same folder as the executable. I'd recommend getting the first 2 dlls from the sites for each library and get `libportaudio.dll` and `libwinpthread-1.dll` from `C:\msys64\mingw64\bin\`.  
However the best solution is probably just to [**get a better operating system.**](https://www.debian.org)  


---

## Basic Structure
### src/engine/ directory
This is where the stuff that doesn't really need to be changed when making a game are (though you might actually still have to edit it at some point because of what a mess this piece of shit is lmao)  
You will need to include the stuff in the directory though in the `src/game` directory to use them.  

#### animation.h and animation.c
Very basic animation handling things using spritesheets.  

#### audio.h and audio.c
A very basic audio synthesizer using PortAudio. You provide the ADSR envelope thing and pointer to the function used for the synthesizer in a synthInstrument structure, and then put that with the start and end frequencies, length and volume into a synthData structure and pass that to the playSynth function. It'd probably be better to just read the header and C files to understand how to use it since I can't really explain this well (though it would probably be best to read through all the files that you're going to be using).  
You can also load samples using `loadWav` and play it using `playSample` too.  

#### entity.h and entity.c
Basically anything that gets updated and/or drawn on screen every frame is an entity. It has a pointer to another struct that has other properties (for sort of polymorphism), 2 function pointers that point to functions that will update and draw the entity that gets called every frame, and a function pointer to destroy any resources being used by the entity when the entity is removed. You can change these function pointers at any time to create a sort of finite state machine (for example you could have it switch from a function to move on the ground to a function to move in the air when jumping)  
Entities also have a list of properties for other things to access easier (such as position), the list has property ids and pointers to where the data for the property is stored.  

#### files.h and files.c
Some very basic file handling for things that aren't considered resources, along with functions to read a gzip or bzip2 compressed file.  

#### logging.h and logging.c
Lets you output to both the terminal and a log file with debug information.  

#### renderer.h and renderer.c
Has very basic functions for drawing rectangles and textures to the screen. I should add more functions like them but that's all there is for now.  

#### resourceManager.h and resourceManager.c
This lets you load resources. If you try to load a resource you've already loaded it will return the pointer to the already loaded resource.  

#### vectors.h and vectors.c
Just defines a bunch of vector types and has a few functions for doing stuff with 2d vectors.  

#### main.c
The entry point of the program. All that's done here is just initializing a bunch of stuff.  

#### src/engine/resourceLoaders/ directory
Where every resource loading and destroying function is defined, you have to update the arrays in `resourceManager.h` and `resourceManager.c`. Currently has support for loading textures and shaders.  

  

### src/external directory
Contains all the parts of the engine that it depends on that aren't dynamically linked. Currently contains GLAD, `stb_image.h`, and optionally PortAudio as a git submodule.  

  

### src/game/ directory
This is where the stuff for the actual game part should be, so stuff like defining game states or whatever  

#### controls.h and controls.c
Has a very basic sort of system for handling key presses without having to check with like GLFW if a key is pressed.  
You need to change the enum in `controls.h` to list all the controls you need and change the array in `controls.c` to change how each key is defined. The array has to be in the same order as the enum.  

#### gameLoop.h and gameLoop.c
Has a while loop that just calls the function in the current game state, updates the delta time and updates the controls. Feels like it should be in the like `src/engine/` directory since it's not really needed to be changed that much for the game but it calls the initialize function of a game state so if you need to change what game state the game starts in you'd have to change it in this file.  

#### gameStates.h and gameStates.c
Defines the gameState type. The `initState` and `uninitState` function pointers point to functions that initialize and uninitialize a state, and the `stateLoop` function pointer points to the function that will be called every frame if that state is the current one. Also could probably be moved to the `src/engine/` directory.  

#### player.h and player.c
A very basic example of an entity.  


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
 - Maybe rewrite the entity system
 - Music playback (probably using amiga mod files since [libopenmpt](https://lib.openmpt.org/libopenmpt/) is a thing)
 - Fix some of the older rendering functions
 - [ciumgui](https://github.com/cimgui/cimgui) integration (requires cmake though)
 - Optimize stuff (the framerate goes down to about 130fps when there's only like 10000 quads of the same texture on screen)
 - Controller support
 - Make the README file not terrible
 - Support for multiple rendering (and maybe audio) backends like vulkan
 - Figure out why btop says it's using over 100MB of memory (valgrind seems to say it's only like 30MB)
 - Add pitch and time stretching for samples (requires weird calculus stuff I don't understand lmao)
 - Fix the weird high pitched frequencies with resampled samples (happens for both playing back at a different sample rate or loading one with a different sample rate)
 - Fix wavy font breaking on windows
 - Maybe get the executable to compile statically
