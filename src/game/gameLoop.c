#include "game/gameLoop.h"

// https://stackoverflow.com/questions/3875197/gcc-with-std-c99-complains-about-not-knowing-struct-timespec
//#define _XOPEN_SOURCE 500

#include <stdio.h>
#include <time.h>

#include "glad/gl.h"
#include "GLFW/glfw3.h"

#include "engine/renderer.h"
#include "engine/entity.h"
#include "engine/resourceManager.h"

#include "game/gameStates.h"
#include "game/gameStates/gameStateRunning.h"
#include "game/controls.h"



int gameLoop(void){
	double lastTime = glfwGetTime();
	double deltaTime = 0.0f;

	uint32_t frameCap = 0;
	struct timespec tim, tim2;
	tim.tv_sec = 0;

	// set key callback
	glfwSetKeyCallback(window, handleKeyEvent);
	glfwSetCursorPosCallback(window, handleMouseMoveEvent);
	glfwSetMouseButtonCallback(window, handleMouseButtonEvent);
	
	initGameState(&gameStateRunning);
	
	while(!glfwWindowShouldClose(window)){
		for(int i = 0; i < CONTROLS_LENGTH; i++){
			if(keys[i].pressedTimer > 0.0){
				keys[i].pressedTimer -= deltaTime;
			}
		}
		
		//clearScreen();
		
		if((*(currentState->stateLoop))(deltaTime)) {running = false;}
		
		if(running == false)  { glfwSetWindowShouldClose(window, GLFW_TRUE); }

		flushAllVertexBuffers();
		
		// swap the frame buffers
		glfwSwapBuffers(window);
		// poll for events
		glfwPollEvents();
		
		// update delta time
		deltaTime = glfwGetTime() - lastTime;
		lastTime = glfwGetTime();

		if(frameCap != 0) {
			tim.tv_nsec = 1000000000L/frameCap - deltaTime*1000000000L;
			nanosleep(&tim, &tim2);
		}
	}
	
	return 0;
}
