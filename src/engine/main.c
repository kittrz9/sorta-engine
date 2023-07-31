#include <stdio.h>
#include <stdlib.h>

#include "renderer.h"

#include "gameLoop.h"
#include "controls.h"
#include "entity.h"
#include "resourceManager.h"
#include "logging.h"
#include "audio.h"

int main(UNUSED int argc, UNUSED char** argv, UNUSED char** envp){
	// initialize the log file
	initLogFile();

	// initialize the resource manager
	initResourceManager();
	
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

	uninitResourceManager();
	
	glfwTerminate();
	
	closeLogFile();
	
	return 0;
}
