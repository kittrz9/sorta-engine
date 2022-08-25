#include <stdio.h>
#include <stdlib.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "renderer.h"

#include "gameLoop.h"
#include "controls.h"
#include "entity.h"
#include "resourceManager.h"
#include "logging.h"
#include "audio.h"

int main(int argc, char** argv){
	// assumes path wont have "build" anywhere, should probably be changed in case someone runs this in a directory that has build but isn't the build directory
	char* c = &argv[0][0];
	char buildstr[] = "build";
	char* c2 = &buildstr[0];
	while(*c != '\0') {
		if(*c2 == *c) {
			++c2;
		}
		if(*c2 == '\0') {
			break;
		}
		++c;
	}
	// only has "build" in the path if being ran outside of the build dir
	if(*c2 != '\0') {
		setResourceDir("../res/");
	} else {
		setResourceDir("res/");
	}

	// initialize the log file
	initLogFile();
	
	// initialize the renderer
	initRenderer();
	
	// initialize the audio
	initAudio();
	
	// call the game loop function (should probably get a better name since the actual part to edit for it is in the game state lmao)
	gameLoop();
	
	// end
	debugLog(LOG_NORMAL, "closing\n");
	
	uninitRenderer();
	uninitAudio();
	
	destroyEntityList();
	clearResourceList();
	
	glfwTerminate();
	
	closeLogFile();
	
	return 0;
}
