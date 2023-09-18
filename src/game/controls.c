#include "controls.h"

#include <stdio.h>

#define INIT_KEY(id, code) [id] = { .held=false, .pressedTimer=0.0f, .keyCode=code },
key keys[CONTROLS_LENGTH] = {
	KEYS_MACRO(INIT_KEY)
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
