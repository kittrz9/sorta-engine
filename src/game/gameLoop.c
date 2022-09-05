#include "gameLoop.h"

#include <stdio.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "renderer.h"
#include "controls.h"
#include "resourceManager.h"

#include "entity.h"
#include "player.h"

#include "gameStates.h"
#include "gameStateRunning.h"

int gameLoop(){
	double lastTime = glfwGetTime();
	double deltaTime = 0.0f;
	
	initGameState(&gameStateRunning);
	
	while(!glfwWindowShouldClose(window)){
		for(int i = 0; i < CONTROLS_LENGTH; i++){
			if(keys[i].pressedTimer > 0.0){
				keys[i].pressedTimer -= deltaTime;
			}
		}
		
		clearScreen();
		
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
	}
	
	return 0;
}
