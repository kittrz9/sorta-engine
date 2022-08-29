#include "renderer.h"

#include <stdio.h>
#include <stdlib.h>

#include "logging.h"
#include "controls.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "resourceManager.h"

int windowWidth  = 640;
int windowHeight = 480;

GLFWwindow* window;

colorRGBA screenClearColor = { 0.3f, 0.3f, 0.3f, 1.0f };

GLint vertexAngleLocation;
GLint vertexRectLocation;
GLint fragmentTextureRectLocation;
GLint fragmentInputColorLocation;
GLint fragmentUseTextureLocation;
GLint fragmentWindowSize;

GLuint* fallbackTexture;

resource* currentShaderRes;
GLuint currentShader;

// could probably use defines to have less repetition
void setShaderUniform1f(const char* name, float value) {
	GLint location = glGetUniformLocation(currentShader, name);
	glUniform1f(location, value);
}

void setShaderUniform2f(const char* name, float value1, float value2) {
	GLint location = glGetUniformLocation(currentShader, name);
	glUniform2f(location, value1, value2);
}

void setShaderUniform4f(const char* name, float value1, float value2, float value3, float value4) {
	GLint location = glGetUniformLocation(currentShader, name);
	glUniform4f(location, value1, value2, value3, value4);
}

void setShaderUniform1ui(const char* name, unsigned int value) {
	GLint location = glGetUniformLocation(currentShader, name);
	glUniform1ui(location, value);
}
void glfwWindowSizeCallback(GLFWwindow* window, int width, int height){
	windowWidth = width;
	windowHeight = height;
	
	glViewport(0,0,width,height);
	
	setShaderUniform2f("windowSize", width, height);
	//printf("window resized to (%i, %i)\n", windowWidth, windowHeight);
	
	return;
}

void glfwErrorCallback(int error, const char* description) {
	debugLog(LOG_ERROR, "GLFW error: %s", description);
}


const float points[] = {
//	vertex coords	texture coords
	-1.0f,  1.0f,	0.0f, 1.0f, // top left
	 1.0f,  1.0f,	1.0f, 1.0f, // top right
	 1.0f, -1.0f,	1.0f, 0.0f, // bottom right
	-1.0f, -1.0f,	0.0f, 0.0f, // bottom left
};

// indices used when drawing triangles
const unsigned int trisIndices[] = {
	0, 1, 2,
	2, 3, 0,
};

// indices used when drawing lines
const unsigned int linesIndices[] = {
	0, 1,
	1, 2,
	2, 3,
	3, 0,
};

// maybe not good to have like one vertex array object but idk it's literally just rotating scaling and translating a square
GLuint vertexBufferObject;
GLuint vertexArrayObject;
GLuint elementBufferObject;

void initRenderer(){
	debugLog(LOG_NORMAL, "initializing renderer\n"); 
	// init GLFW
	debugLog(LOG_NORMAL, "initializing GLFW\n");
	if(!glfwInit()) {
		debugLog(LOG_ERROR, "could not initialize GLFW\n");
		exit(-1);
	}
	debugLog(LOG_SUCCESS, "glfw successfully initialized\n");

	// create window
	debugLog(LOG_NORMAL, "creating window\n");
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	glfwWindowHint(GLFW_FLOATING, GLFW_TRUE);
	window = glfwCreateWindow(windowWidth, windowHeight, "bruh", NULL, NULL);
	if(!window) {
		debugLog(LOG_ERROR, "could not create window\n");
		glfwTerminate();
		exit(-1);
	}
	glfwMakeContextCurrent(window);
	debugLog(LOG_SUCCESS, "window successfully created\n");

	// init glew
	debugLog(LOG_NORMAL, "initializing glew\n");
	glewExperimental = GL_TRUE;
	GLenum error = glewInit();
	if(error != GLEW_OK) {
		debugLog(LOG_ERROR, "could not initialize glew, %s\n", glewGetErrorString(error));
		exit(-1);
	}
	debugLog(LOG_SUCCESS, "glew successfully initialized\n");

	// enable blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	const GLubyte* renderer = glGetString(GL_RENDERER);
	const GLubyte* version  = glGetString(GL_VERSION);
	debugLog(LOG_NORMAL, "Renderer: %s, OpenGL version: %s\n", renderer, version);

	// turn off vsync
	glfwSwapInterval(0);

	// set window size callback
	glfwSetWindowSizeCallback(window, glfwWindowSizeCallback);

	// set key callback
	glfwSetKeyCallback(window, handleKeyEvent);
	glfwSetErrorCallback(glfwErrorCallback);

	// set up vertex buffer object
	debugLog(LOG_NORMAL, "setting up vertex buffer object\n");
	glGenBuffers(1, &vertexBufferObject);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
	glBufferData(GL_ARRAY_BUFFER, (sizeof(points)/sizeof(points[0])) * sizeof(float), points, GL_STATIC_DRAW);
	
	// set up vertex array object
	debugLog(LOG_NORMAL, "setting up vertex array object\n");
	glGenVertexArrays(1,&vertexArrayObject);
	glBindVertexArray(vertexArrayObject);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), NULL);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)(2*sizeof(float)));
	glEnableVertexAttribArray(1);
	debugLog(LOG_SUCCESS, "successfully set up vertex array object\n");
	
	// set up element buffer object
	glGenBuffers(1, &elementBufferObject);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferObject);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(trisIndices), trisIndices, GL_STATIC_DRAW);
	debugLog(LOG_SUCCESS, "successfully set up element buffer object\n");
	
	// compile shader 
	// need to have a default shader to set the windowSize uniform
	debugLog(LOG_NORMAL, "compiling default shader\n");
	resource* shaderResource = loadShader("defaultShader", "shaders/vertexShader.glsl", "shaders/fragmentShader.glsl");

	useShader(shaderResource);

	setShaderUniform2f("windowSize", windowWidth, windowHeight);

	debugLog(LOG_NORMAL, "setting up fallback texture\n");
	// probably could just set up a different function to set up a texture so I don't repeat the code in resourceManager.c but whatever
	fallbackTexture = malloc(sizeof(GLuint));

	glGenTextures(1, fallbackTexture);
	glBindTexture(GL_TEXTURE_2D, *fallbackTexture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// hardcoded values that probably shouldn't be hardcoded lmao
	const unsigned char data[] = {0xFF, 0x00, 0xFF, 0xFF,  0x00, 0x00, 0x00, 0xFF,
				      0x00, 0x00, 0x00, 0xFF,  0xFF, 0x00, 0xFF, 0xFF};
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 2, 2, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);
	debugLog(LOG_SUCCESS, "fallback texture successfully set up\n");
	
	debugLog(LOG_SUCCESS, "renderer successfully initailized\n");
	
	return;
}

void uninitRenderer(){
	glDeleteVertexArrays(1, &vertexArrayObject);
	glDeleteBuffers(1, &vertexBufferObject);

	glDeleteTextures(1, ((GLuint*)fallbackTexture));
	
	return;
}

void changeClearScreenColor(colorRGBA color) {
	screenClearColor = color;
}

void clearScreen() {
	glClear(GL_COLOR_BUFFER_BIT);
	glClearColor(screenClearColor.r, screenClearColor.g, screenClearColor.b, screenClearColor.a);
}

// probably not really necessary for having support for multiple shader but it'll probably make supporting another backend like vulkan easier
void useShader(resource* shaderRes) {
	if(shaderRes->type != RES_TYPE_SHADER) {
		debugLog(LOG_ERROR, "resource \"%s\" is not a shader\n", shaderRes->name);
		return;
	}

	if(currentShaderRes == shaderRes) {
		return;
	}
	
	currentShader = *(GLuint*)shaderRes->pointer;
	glUseProgram(currentShader);
	currentShaderRes = shaderRes;
}

// size is between -1 and 1 like the shader or whatever, has to be converted from like world space or whatever into screen space
void drawFilledRect(rect drawnRect, colorRGBA color, float angle){
	setShaderUniform1f("angle", angle);
	setShaderUniform4f("rect", drawnRect.x, drawnRect.y, drawnRect.w, drawnRect.h);
	setShaderUniform4f("inputColor", color.r, color.g, color.b, color.a);
	setShaderUniform1ui("useTexture", GL_FALSE);
	
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(trisIndices), trisIndices, GL_STATIC_DRAW);
	glDrawElements(GL_TRIANGLES, sizeof(trisIndices)/sizeof(trisIndices[0]), GL_UNSIGNED_INT, 0);
	
	return;
}

void drawLineRect(rect drawnRect, colorRGBA color, float angle){
	setShaderUniform1f("angle", angle);
	setShaderUniform4f("rect", drawnRect.x, drawnRect.y, drawnRect.w, drawnRect.h);
	setShaderUniform4f("inputColor", color.r, color.g, color.b, color.a);
	setShaderUniform1ui("useTexture", GL_FALSE);
	
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(linesIndices), linesIndices, GL_STATIC_DRAW);
	glDrawElements(GL_LINES, sizeof(linesIndices), GL_UNSIGNED_INT, 0);
	
	return;
}

void drawTexture(rect drawnRect, rect textureRect, colorRGBA color, float angle, resource* textureRes){
	if(textureRes->type != RES_TYPE_TEXTURE) {
		debugLog(LOG_ERROR, "resource  \"%s\" is not a texture\n", textureRes->name);
		exit(1);
	}

	setShaderUniform1f("angle", angle);
	setShaderUniform4f("rect", drawnRect.x, drawnRect.y, drawnRect.w, drawnRect.h);
	if(textureRes->pointer == fallbackTexture) {
		// hardcoded values again lmao
		setShaderUniform4f("rect", 0,0,2,2);
	} else {
		setShaderUniform4f("rect", drawnRect.x, drawnRect.y, drawnRect.w, drawnRect.h);
		setShaderUniform4f("textureRect", textureRect.x, textureRect.y, textureRect.w, textureRect.h);
	}
	setShaderUniform4f("inputColor", color.r, color.g, color.b, color.a);
	setShaderUniform1ui("useTexture", GL_TRUE);
	
	glBindTexture(GL_TEXTURE_2D, *(GLuint*)textureRes->pointer);
	
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(trisIndices), trisIndices, GL_STATIC_DRAW);
	glDrawElements(GL_TRIANGLES, sizeof(trisIndices)/sizeof(trisIndices[0]), GL_UNSIGNED_INT, 0);
	
	return;
}

void drawLines(const float* linePoints, unsigned int count, colorRGBA color) {
	setShaderUniform1f("angle", 0);
	setShaderUniform4f("rect", 0,0,1,1);
	setShaderUniform4f("inputColor", color.r, color.g, color.b, color.a);
	setShaderUniform1ui("useTexture", GL_FALSE);
	
	// maybe not a good thing to constantly change the vertex attributes thing every frame just to draw a line 
	// but it's doing this so you don't have to define the texture coordinates with the line points
	// I don't think it affected performance that much though I'm probably wrong
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), NULL);
	glEnableVertexAttribArray(0);
	
	// change the data being used for drawing
	glBufferData(GL_ARRAY_BUFFER, count * sizeof(float), linePoints, GL_STATIC_DRAW);
	
	glDrawArrays(GL_LINES, 0, count);
	
	// change it back
	glBufferData(GL_ARRAY_BUFFER, (sizeof(points)/sizeof(points[0])) * sizeof(float), points, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), NULL);
	return;
}

// repeated code lmao, should probably be made better eventually
void drawTriangles(const float* triPoints, unsigned int count, colorRGBA color) {
	setShaderUniform1f("angle", 0);
	setShaderUniform4f("rect", 0,0,1,1);
	setShaderUniform4f("inputColor", color.r, color.g, color.b, color.a);
	setShaderUniform1ui("useTexture", GL_FALSE);
	
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), NULL);
	glEnableVertexAttribArray(0);
	
	// change the data being used for drawing
	glBufferData(GL_ARRAY_BUFFER, count * sizeof(float), triPoints, GL_STATIC_DRAW);
	
	glDrawArrays(GL_TRIANGLES, 0, count);
	
	// change it back
	glBufferData(GL_ARRAY_BUFFER, (sizeof(points)/sizeof(points[0])) * sizeof(float), points, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), NULL);
	return;
}
