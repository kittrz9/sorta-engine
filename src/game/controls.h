#ifndef CONTROLS_H
#define CONTROLS_H

#include <stdbool.h>

#include "GLFW/glfw3.h"

typedef struct {
	bool held;
	float pressedTimer;
	int keyCode;
} key;

// https://en.wikipedia.org/wiki/X_macro
#define KEYS_MACRO(DO) \
DO(KEY_EXIT,  GLFW_KEY_ESCAPE) \
DO(KEY_UP,    GLFW_KEY_UP) \
DO(KEY_DOWN,  GLFW_KEY_DOWN) \
DO(KEY_LEFT,  GLFW_KEY_LEFT) \
DO(KEY_RIGHT, GLFW_KEY_RIGHT) \

#define KEY_DEFINITION(id, ...) id,
enum CONTROLS {
	KEYS_MACRO(KEY_DEFINITION)
	CONTROLS_LENGTH,
};
#undef KEY_DEFINITION

extern key keys[CONTROLS_LENGTH];

// glfw key event callback
void handleKeyEvent(GLFWwindow* window, int key, int scancode, int action, int mods);

enum {
	MOUSE_LEFT   = 1 << GLFW_MOUSE_BUTTON_LEFT,
	MOUSE_RIGHT  = 1 << GLFW_MOUSE_BUTTON_RIGHT,
	MOUSE_MIDDLE = 1 << GLFW_MOUSE_BUTTON_MIDDLE,
};

struct mouseStruct {
	float x, y;
	uint8_t buttons;
};

extern struct mouseStruct mouse;

void handleMouseMoveEvent(GLFWwindow* window, double xpos, double ypos);
void handleMouseButtonEvent(GLFWwindow* window, int button, int action, int mods);

#endif
