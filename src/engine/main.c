#include <stdio.h>
#include <stdlib.h>

#include "engine/renderer.h"
#include "engine/entity.h"
#include "engine/resourceManager.h"
#include "engine/logging.h"
#include "engine/audio.h"

#include "game/gameLoop.h"

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
	
	destroyEntityList();
	
	uninitAudio();

	uninitResourceManager();
	
	uninitRenderer();

	closeLogFile();
	
	return 0;
}
