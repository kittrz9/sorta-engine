#include <stdio.h>
#include <stdlib.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "renderer.h"

#include "gameLoop.h"
#include "controls.h"
#include "entity.h"
#include "resourceManager.h"

int main(){
	// init GLFW
	if(!glfwInit()){
		printf("could not initialized GLFW\n");
		return -1;
	}
	
	// create window
	window = glfwCreateWindow(windowWidth, windowHeight, "bruh", NULL, NULL);
	if(!window){
		printf("could not create window\n");
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
	printf("Renderer: %s\nOpenGL version: %s\n", renderer, version);
	
	// turn on vsync
	// for some reason I was getting screen tearing with vsync on and turning it off fixed it, makes literally no sense but whatever lmao
	glfwSwapInterval(0);
	
	// set window size callback
	glfwSetWindowSizeCallback(window, glfwWindowSizeCallback);
	
	// set key callback
	glfwSetKeyCallback(window, handleKeyEvent);
	
	// initialize the renderer
	initRenderer();
	
	// call the game loop function (should probably get a better name since the actual part to edit for it is in the game state lmao)
	gameLoop();
	
	// end
	printf("closing\n");
	
	destroyEntityList();
	clearResourceList();
	
	glfwTerminate();
	
	return 0;
}
