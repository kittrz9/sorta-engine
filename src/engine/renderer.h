#ifndef RENDERER_H
#define RENDERER_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "resourceManager.h"

extern int windowWidth;
extern int windowHeight;
extern GLFWwindow* window;

void glfwWindowSizeCallback(GLFWwindow* window, int width, int height);

typedef struct {
	float x,y,w,h;
} rect;

typedef struct {
	float r,g,b,a;
} colorRGBA;

// color that the screen will be clear to
extern colorRGBA screenClearColor;

void initRenderer();
void uninitRenderer();

// location of the uniform things in the shaders
extern GLint vertexAngleLocation;
extern GLint vertexRectLocation;
extern GLint fragmentTextureRectLocation;
extern GLint fragmentInputColorLocation;
// there's probably a better way to make the drawRect function not use a texture
extern GLint fragmentUseTextureLocation;
extern GLint fragmentWindowSize;

void drawFilledRect(rect drawnRect, colorRGBA color, float angle);
void drawLineRect(rect drawnRect, colorRGBA color, float angle);
// pointer to the resource instead of the texture itself since it makes it so you don't have to do weird like casts and pointer stuff when calling the function
// drawn rect is the size and position of the rect to be drawn to the screen, in screen coordinates (both in the range of -1 to 1)
// texture rect is the size and position of the part of the texture to be put onto the screen, in coordinates that correspond with the pixels of the texture ( for example (0,10,20,20) will crop out the part of the texture 20x20 pixels large and 10 pixels below the top left
void drawTexture(rect drawnRect, rect textureRect, colorRGBA color, float angle, resource* texture);

void drawLines(const float* linePoints, unsigned int count, colorRGBA color);

#endif
