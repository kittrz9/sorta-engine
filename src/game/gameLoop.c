#include "game/gameLoop.h"

// https://stackoverflow.com/questions/3875197/gcc-with-std-c99-complains-about-not-knowing-struct-timespec
//#define _XOPEN_SOURCE 500

#include <stdio.h>
#include <time.h>

#include "glad/gl.h"
#include "GLFW/glfw3.h"

#include "engine/window.h"
#include "engine/renderer.h"
#include "engine/entity.h"
#include "engine/resourceManager.h"

#include "game/gameStates.h"
#include "game/gameStates/gameStateRunning.h"
#include "game/controls.h"

#define USING_GLFW

void initGame(double* lastTime) {
#ifdef USING_GLFW
	*lastTime = glfwGetTime();

	// set key callback
	glfwSetKeyCallback(window, handleKeyEvent);
	glfwSetCursorPosCallback(window, handleMouseMoveEvent);
	glfwSetMouseButtonCallback(window, handleMouseButtonEvent);
#endif

	return;
}

void endFrame(double* deltaTime, double* lastTime, uint32_t frameCap) {
	flushAllVertexBuffers();
	

#ifdef USING_GLFW
	static struct timespec tim, tim2;
	tim.tv_sec = 0;

	// swap the frame buffers
	glfwSwapBuffers(window);
	// poll for events
	glfwPollEvents();
	
	// update delta time
	*deltaTime = glfwGetTime() - *lastTime;
	*lastTime = glfwGetTime();

	if(frameCap != 0) {
		tim.tv_nsec = 1000000000L/frameCap - *deltaTime*1000000000L;
		nanosleep(&tim, &tim2);
	}

	if(running == false)  { glfwSetWindowShouldClose(window, GLFW_TRUE); }

#endif

	return;
}


int gameLoop(void){
	double lastTime;
	double deltaTime = 0.0f;

	uint32_t frameCap = 0;

	initGame(&lastTime);

	initGameState(&gameStateRunning);
	
	while(!glfwWindowShouldClose(window)){
		for(int i = 0; i < CONTROLS_LENGTH; i++){
			if(keys[i].pressedTimer > 0.0){
				keys[i].pressedTimer -= deltaTime;
			}
		}
		
		if((*(currentState->stateLoop))(deltaTime)) {running = false;}
		
		endFrame(&deltaTime, &lastTime, frameCap);
	}
	
	return 0;
}

