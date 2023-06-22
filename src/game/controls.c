#include "controls.h"

#include <stdio.h>

key keys[CONTROLS_LENGTH] = {
	[KEY_EXIT] = {
		.name="exit",
		.held=false,
		.pressedTimer=0.0f,
		.keyCode=GLFW_KEY_ESCAPE,
	},
	[KEY_UP] = {
		.name="up",
		.held=false,
		.pressedTimer=0.0f,
		.keyCode=GLFW_KEY_UP,
	},
	[KEY_DOWN] = {
		.name="down",
		.held=false,
		.pressedTimer=0.0f,
		.keyCode=GLFW_KEY_DOWN,
	},
	[KEY_LEFT] = {
		.name="left",
		.held=false,
		.pressedTimer=0.0f,
		.keyCode=GLFW_KEY_LEFT,
	},
	[KEY_RIGHT] = {
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
