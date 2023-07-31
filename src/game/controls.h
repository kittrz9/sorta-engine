#ifndef CONTROLS_H
#define CONTROLS_H

#include <stdbool.h>

#include <GLFW/glfw3.h>

typedef struct {
	bool held;
	float pressedTimer;
	int keyCode;
	const char* name;
} key;

enum CONTROLS{KEY_EXIT, KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT, CONTROLS_LENGTH};
extern key keys[CONTROLS_LENGTH];

// glfw key event callback
void handleKeyEvent(GLFWwindow* window, int key, int scancode, int action, int mods);

#endif
