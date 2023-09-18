#include "controls.h"

#include <stdio.h>

#define DEFINE_KEY(id, code) [id] = { .held=false, .pressedTimer=0.0f, .keyCode=code }

key keys[CONTROLS_LENGTH] = {
	DEFINE_KEY(KEY_EXIT, GLFW_KEY_ESCAPE),
	DEFINE_KEY(KEY_UP, GLFW_KEY_UP),
	DEFINE_KEY(KEY_DOWN, GLFW_KEY_DOWN),
	DEFINE_KEY(KEY_LEFT, GLFW_KEY_LEFT),
	DEFINE_KEY(KEY_RIGHT, GLFW_KEY_RIGHT),
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
