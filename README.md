# sorta engine?
This is kinda a 2d game engine (I think? idk what classifies it as like either a game engine or framework or whatever the fuck this thing is)<br>

All of the stuff I've seen that tries to define what is a game engine or a framework or whatever seems to contradict a lot and I am very confused lmao. This feels somewhere in between a framework like SDL and an engine. You don't like include this as like a library like you do with SDL, but a lot of stuff isn't done for you like physics and other things (mostly because I feel like if I do that it would make this thing a lot less useful since it would limit it to only one kind of game or whatever without major modifications or work arounds.). There also isn't any like tools made for this (yet) so I guess you could be like "oh if it doesn't have a level editor or whatever it's not a game engine" though I think that's kinda stupid.<br>

Though I feel like this classifies as an engine in the way you can say a game that was built off of another game "uses the same engine" even if they don't use something like unreal or unity or whatever.<br>

Who knows, maybe I'll work on this for long enough this becomes an actual full game engine and I wont have to be like "it might be one idk". lmao<br>

To make stuff you should be able to just edit stuff in the `src/game/` directory to make things.<br>

I'll hopefully be able to use this as sort of a base for other projects/games (if I ever make them lmao)<br>

This is based off of the other thing I made that I'm not sure classifies as a game engine or not (<https://github.com/kittrz9/SDL_ProjectBase>)<br>

As of now this only has support for Linux, though I'd assume it's not too hard to make things compile for windows.<br>

This is pretty much just a project I'm doing for fun/learning and I doubt anyone actually would want to use this, though if for some strange reason you want to I'm fine with it. All I'd really want is just to be credited but I'm fine as long as you don't claim this stuff as your own. This project uses the Unlicense so whatever, I don't really care that much.<br>

---

## Compiling
All you should need to compile this on Linux is glew and glfw3, both of which should be available from the package manager of whatever distro you're using.<br>
Then all you'd need to do to compile this is run `make` and it should compile into the `build/` directory.<br>

---

## Basic Structure
### src/engine/ directory
This is where the stuff that doesn't really need to be changed when making a game are (though you might actually still have to edit it at some point because of what a mess this piece of shit is lmao)<br>
You will need to include the stuff in the directory though in the `src/game` directory to use them.<br>

#### animation.h and animation.c
Very basic animation handling things using spritesheets.<br>

#### entity.h and entity.c
Basically anything that gets updated and/or drawn on screen every frame is an entity. It has a pointer to another struct that has other properties (for sort of polymorphism), 2 function pointers that point to functions that will update and draw the entity that gets called every frame, and a function pointer to destroy any resources being used by the entity when the entity is removed.<br>

#### renderer.h and renderer.c
Has very basic functions for drawing rectangles and textures to the screen. I should add more functions like them but that's all there is for now.<br>

#### resourceManager.h and resourceManager.c
This lets you load resources. If you try to load a resource you've already loaded it will return the pointer to the already loaded resource. As of now it only has support for loading textures but it should be easy to add new resource types.<br>

#### shader.h and shader.c
Used for loading the shader. There is only one shader so far. This really should be put into the resource manager so you can use multiple shaders but whatever I don't want to do that right now.<br>

#### stb_image.h
Not written by me. It's a public domain image library gotten from here: <https://github.com/nothings/stb/blob/master/stb_image.h><br>
It's here so I can load textures easily.<br>

#### vectors.h and vectors.c
Just defines a bunch of vector types and has a few functions for doing stuff with 2d vectors.<br>

#### main.c
The entry point of the program. All that's done here is just initializing a bunch of stuff.<br>

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
Where the resources for the game should be. For now it only has a place holder image used for the player.

### shaders/ directory
Where the shaders should be. Right now only has one vertex and fragment shader.

---

## Major things to do (in no particular order):

 - Audio (And synthesizer)
 - Support for multiple shaders
 - More rendering functions
 - ciumgui integration (<https://github.com/cimgui/cimgui>)
 - Optimize stuff
 - Text rendering
 - Being able to read from a config file
 - Windows support
