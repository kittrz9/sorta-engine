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

int main(){
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
