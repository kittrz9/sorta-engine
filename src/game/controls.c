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

void handleKeyEvent(UNUSED GLFWwindow* window, int key, UNUSED int scancode, int action, UNUSED int mods) {
	for(int i = 0; i < CONTROLS_LENGTH; i++){
		if(keys[i].keyCode == key){
			if(action == GLFW_PRESS){
				keys[i].held = true;
				keys[i].pressedTimer = 0.1f;
			} else if(action == GLFW_RELEASE){
				keys[i].held = false;
				keys[i].pressedTimer = 0.0f;
			}
		}
	}
	
	return;
}
