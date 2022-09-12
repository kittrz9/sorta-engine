#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
	char buildstr[] = "build";
	unsigned int i = 0;
	unsigned int flag = 0;
	for(i = 0; argv[0][i] != '\0'; ++i) {
		if(strncmp(argv[0]+i, buildstr, strlen(buildstr)) == 0) {
			flag = 1;
			break;
		}
	}
	// only has "build" in the path if being ran outside of the build dir
	char* str = malloc(256 * sizeof(char));
	if(flag) {
		strncpy(str, argv[0], i);
		strcat(str, "res/");
	} else {
		sprintf(str, "res/");
	}
	setResourceDir(str);
	free(str);

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
