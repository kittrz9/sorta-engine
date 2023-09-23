#include "engine/window.h"

#include <stdlib.h>

#include "engine/logging.h"
#include "engine/renderer.h"

#include "GLFW/glfw3.h"

int windowWidth  = 640;
int windowHeight = 480;

SORTA_WINDOW window;

void glfwWindowSizeCallback(UNUSED GLFWwindow* window, int width, int height) {
	windowWidth = width;
	windowHeight = height;

	glViewport(0,0,width,height);

	setShaderUniform2f("windowSize", width, height);

	return;
}

void glfwErrorCallback(int error, const char* description) {
	debugLog(LOG_ERROR, "GLFW error %i: \"%s\"\n", error, description);

	return;
}

void initWindow(void) {
	const char* errorDescription;
	int errorCode;
	debugLog(LOG_NORMAL, "initializing GLFW\n");
	if(!glfwInit()) {
		errorCode = glfwGetError(&errorDescription);
		debugLog(LOG_ERROR, "could not initialize GLFW, GLFW Error #%i: \"%s\"\n", errorCode, errorDescription);
		exit(1);
	}
	debugLog(LOG_SUCCESS, "GLFW successfully initialized\n");

	debugLog(LOG_NORMAL, "creating window\n");
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	glfwWindowHint(GLFW_FLOATING, GLFW_TRUE);
	window = glfwCreateWindow(windowWidth, windowHeight, "bruh", NULL, NULL);
	if(!window) {
		errorCode = glfwGetError(&errorDescription);
		debugLog(LOG_ERROR, "could not initialize GLFW, GLFW Error #%i: \"%s\"\n", errorCode, errorDescription);
		glfwTerminate();
		exit(1);
	}
	glfwMakeContextCurrent(window);
	debugLog(LOG_SUCCESS, "window successfully created\n");

	// turn off vsync
	glfwSwapInterval(0);

	// set window size callback
	glfwSetWindowSizeCallback(window, glfwWindowSizeCallback);

	// set key callback
	glfwSetErrorCallback(glfwErrorCallback);

	return;
}

void destroyWindow(void) {
	glfwTerminate();
}

