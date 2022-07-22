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
	
	// should probably move this stuff to the function to initialize the renderer
	// init GLFW
	if(!glfwInit()){
		debugLog(LOG_ERROR, "could not initialize GLFW\n");
		return -1;
	}
	
	// create window
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	glfwWindowHint(GLFW_FLOATING, GLFW_TRUE);
	window = glfwCreateWindow(windowWidth, windowHeight, "bruh", NULL, NULL);
	if(!window){
		debugLog(LOG_ERROR, "Could not create window\n");
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	
	// init glew
	glewExperimental = GL_TRUE;
	glewInit();
	
	// enable blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	// get version stuff
	const GLubyte* renderer = glGetString(GL_RENDERER);
	const GLubyte* version  = glGetString(GL_VERSION);
	debugLog(LOG_NORMAL, "Renderer: %s, OpenGL version: %s\n", renderer, version);
	
	// turn on vsync
	// for some reason I was getting screen tearing with vsync on and turning it off fixed it, makes literally no sense but whatever lmao
	glfwSwapInterval(0);
	
	// set window size callback
	glfwSetWindowSizeCallback(window, glfwWindowSizeCallback);
	
	// set key callback
	glfwSetKeyCallback(window, handleKeyEvent);
	
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
