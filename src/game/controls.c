#include "controls.h"

#include <stdio.h>

key keys[CONTROLS_LENGTH] = {
	{
		.name="exit",
		.held=false,
		.pressedTimer=0.0f,
		.keyCode=GLFW_KEY_ESCAPE,
	},
	{
		.name="up",
		.held=false,
		.pressedTimer=0.0f,
		.keyCode=GLFW_KEY_UP,
	},
	{
		.name="down",
		.held=false,
		.pressedTimer=0.0f,
		.keyCode=GLFW_KEY_DOWN,
	},
	{
		.name="left",
		.held=false,
		.pressedTimer=0.0f,
		.keyCode=GLFW_KEY_LEFT,
	},
	{
		.name="right",
		.held=false,
		.pressedTimer=0.0f,
		.keyCode=GLFW_KEY_RIGHT,
	},
};

void handleKeyEvent(GLFWwindow* window, int key, int scancode, int action, int mods) {
	for(int i = 0; i < CONTROLS_LENGTH; i++){
		if(keys[i].keyCode == key){
			printf("key \"%s\" ", keys[i].name);
			if(action == GLFW_PRESS){
				printf("pressed\n");
				keys[i].held = true;
				keys[i].pressedTimer = 0.1f;
			} else if(action == GLFW_RELEASE){
				printf("released\n");
				keys[i].held = false;
				keys[i].pressedTimer = 0.0f;
			}
		}
	}
	
	return;
}
