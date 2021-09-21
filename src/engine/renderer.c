#include "renderer.h"

#include <stdio.h>

#include "shader.h"
#include "logging.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

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

void glfwWindowSizeCallback(GLFWwindow* window, int width, int height){
	windowWidth = width;
	windowHeight = height;
	
	glViewport(0,0,width,height);
	
	glUniform2f(fragmentWindowSize, width, height);
	//printf("window resized to (%i, %i)\n", windowWidth, windowHeight);
	
	return;
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
	
	// set up element buffer object
	glGenBuffers(1, &elementBufferObject);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferObject);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(trisIndices), trisIndices, GL_STATIC_DRAW);
	
	// compile shader 
	debugLog(LOG_NORMAL, "compiling shaders\n");
	GLuint shaderProgram = createShader("shaders/vertexShader.glsl", "shaders/fragmentShader.glsl");
	
	// this will probably be really bad for later if I ever want to use other shaders
	glUseProgram(shaderProgram);
	
	// set up the uniform things in the shaders
	vertexAngleLocation = glGetUniformLocation(shaderProgram, "angle");
	vertexRectLocation = glGetUniformLocation(shaderProgram, "rect");
	fragmentTextureRectLocation = glGetUniformLocation(shaderProgram, "textureRect");
	fragmentInputColorLocation = glGetUniformLocation(shaderProgram, "inputColor");
	fragmentUseTextureLocation = glGetUniformLocation(shaderProgram, "useTexture");
	fragmentWindowSize = glGetUniformLocation(shaderProgram, "windowSize");
	
	glUniform2f(fragmentWindowSize, windowWidth, windowHeight);
	
	debugLog(LOG_SUCCESS, "renderer successfully initailized\n");
	
	return;
}

void uninitRenderer(){
	glDeleteVertexArrays(1, &vertexArrayObject);
	glDeleteBuffers(1, &vertexBufferObject);
	
	return;
}


// size is between -1 and 1 like the shader or whatever, has to be converted from like world space or whatever into screen space
void drawFilledRect(rect drawnRect, colorRGBA color, float angle){
	glUniform1f(vertexAngleLocation, angle);
	glUniform4f(vertexRectLocation, drawnRect.x, drawnRect.y, drawnRect.w, drawnRect.h);
	glUniform4f(fragmentInputColorLocation, color.r, color.g, color.b, color.a);
	glUniform1ui(fragmentUseTextureLocation, GL_FALSE);
	
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(trisIndices), trisIndices, GL_STATIC_DRAW);
	glDrawElements(GL_TRIANGLES, sizeof(trisIndices)/sizeof(trisIndices[0]), GL_UNSIGNED_INT, 0);
	
	return;
}

void drawLineRect(rect drawnRect, colorRGBA color, float angle){
	glUniform1f(vertexAngleLocation, angle);
	glUniform4f(vertexRectLocation, drawnRect.x, drawnRect.y, drawnRect.w, drawnRect.h);
	glUniform4f(fragmentInputColorLocation, color.r, color.g, color.b, color.a);
	glUniform1ui(fragmentUseTextureLocation, GL_FALSE);
	
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(linesIndices), linesIndices, GL_STATIC_DRAW);
	glDrawElements(GL_LINES, sizeof(linesIndices), GL_UNSIGNED_INT, 0);
	
	return;
}

void drawTexture(rect drawnRect, rect textureRect, colorRGBA color, float angle, resource* texture){
	glUniform1f(vertexAngleLocation, angle);
	glUniform4f(vertexRectLocation, drawnRect.x, drawnRect.y, drawnRect.w, drawnRect.h);
	glUniform4f(fragmentTextureRectLocation, textureRect.x, textureRect.y, textureRect.w, textureRect.h);
	glUniform4f(fragmentInputColorLocation, color.r, color.g, color.b, color.a);
	glUniform1ui(fragmentUseTextureLocation, GL_TRUE);
	
	glBindTexture(GL_TEXTURE_2D, *(GLuint*)texture->pointer);
	
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(trisIndices), trisIndices, GL_STATIC_DRAW);
	glDrawElements(GL_TRIANGLES, sizeof(trisIndices)/sizeof(trisIndices[0]), GL_UNSIGNED_INT, 0);
	
	return;
}

void drawLines(const float* linePoints, unsigned int count, colorRGBA color) {
	glUniform1f(vertexAngleLocation, 0);
	glUniform4f(vertexRectLocation, 0, 0, 1, 1);
	glUniform4f(fragmentInputColorLocation, color.r, color.g, color.b, color.a);
	glUniform1ui(fragmentUseTextureLocation, GL_FALSE);
	
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
	glUniform1f(vertexAngleLocation, 0);
	glUniform4f(vertexRectLocation, 0, 0, 1, 1);
	glUniform4f(fragmentInputColorLocation, color.r, color.g, color.b, color.a);
	glUniform1ui(fragmentUseTextureLocation, GL_FALSE);
	
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
