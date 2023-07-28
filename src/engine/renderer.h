#ifndef RENDERER_H
#define RENDERER_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "resourceManager.h"
#include "vectors.h"

extern int windowWidth;
extern int windowHeight;
extern GLFWwindow* window;

void glfwWindowSizeCallback(GLFWwindow* window, int width, int height);

typedef struct {
	float x,y,w,h;
} rect;

// WARNING: INCREDIBLE JANKINESS
//
// tinycc will just sometimes decide certain colors just don't get passed into drawText,
//
// BUT IF IT IS A DOUBLE IT WORKS??????
//
// either the compiler or this entire project is cursed.
typedef struct {
#ifdef __TINYC__
	double r,g,b,a;
#else
	float r,g,b,a;
#endif
} colorRGBA;

typedef struct {
	vec2f position;
	vec2f texCoords;
	colorRGBA color;
} vertex;

typedef struct {
	GLuint bufferID;
	unsigned int loadedVertices;
	GLenum mode; // things like GL_STATIC_DRAW and GL_DYNAMIC_DRAW
} vertexBuffer;

// draw a vertex buffer to the screen and clear the buffer (really only for dynamic buffers)
void flushVertexBuffer(vertexBuffer* buf);
// called at the end of the frame
void flushAllVertexBuffers(void);

// color that the screen will be clear to
extern colorRGBA screenClearColor;

void initRenderer(void);
void uninitRenderer(void);

// location of the uniform things in the shaders
extern GLint vertexAngleLocation;
extern GLint vertexRectLocation;
extern GLint fragmentTextureRectLocation;
extern GLint fragmentInputColorLocation;
// there's probably a better way to make the drawRect function not use a texture
extern GLint fragmentUseTextureLocation;
extern GLint fragmentWindowSize;

void setShaderUniform1f(const char* name, float value);
void setShaderUniform2f(const char* name, float value1, float value2);
void setShaderUniform3f(const char* name, float value1, float value2, float value3);
void setShaderUniform4f(const char* name, float value1, float value2, float value3, float value4);
void setShaderUniform1ui(const char* name, unsigned int value);

void changeClearScreenColor(colorRGBA color);
void clearScreen(void);

void useShader(resource* shaderRes);

void drawFilledRect(rect drawnRect, colorRGBA color, float angle);
void drawLineRect(rect drawnRect, colorRGBA color, float angle);
// pointer to the resource instead of the texture itself since it makes it so you don't have to do weird like casts and pointer stuff when calling the function
// drawn rect is the size and position of the rect to be drawn to the screen, in screen coordinates (both in the range of -1 to 1)
// texture rect is the size and position of the part of the texture to be put onto the screen, in coordinates that correspond with the pixels of the texture ( for example (0,10,20,20) will crop out the part of the texture 20x20 pixels large and 10 pixels below the top left
void drawTexture(rect drawnRect, rect textureRect, colorRGBA color, float angle, resource* texture);

void drawLines(const float* linePoints, unsigned int count, colorRGBA color);
void drawTriangles(const float* triPoints, unsigned int count, colorRGBA color);

typedef enum {
	TEXT_ALIGN_LEFT,
	TEXT_ALIGN_CENTER,
	TEXT_ALIGN_RIGHT,
} TEXT_ALIGN;
void drawText(resource* fontRes, char* text, float size, colorRGBA color, float x, float y, TEXT_ALIGN align);
float getTextWidth(resource* fontRes, char* text, float size);

static const colorRGBA COLOR_RGBA_WHITE  = {1.0f,1.0f,1.0f,1.0f};
static const colorRGBA COLOR_RGBA_RED    = {1.0f,0.0f,0.0f,1.0f};
static const colorRGBA COLOR_RGBA_GREEN  = {0.0f,1.0f,0.0f,1.0f};
static const colorRGBA COLOR_RGBA_BLUE   = {0.0f,0.0f,1.0f,1.0f};

#endif
