#include "gameLoop.h"

#include <stdio.h>
#include <time.h>

#include "glad/gl.h"
#include "GLFW/glfw3.h"

#include "renderer.h"
#include "controls.h"
#include "resourceManager.h"

#include "entity.h"
#include "player.h"

#include "gameStates.h"
#include "gameStateRunning.h"

int gameLoop(void){
	double lastTime = glfwGetTime();
	double deltaTime = 0.0f;

	uint32_t frameCap = 0;
	struct timespec tim, tim2;
	tim.tv_sec = 0;
	
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
