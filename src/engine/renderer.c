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

void glfwWindowSizeCallback(GLFWwindow* window, int width, int height){
	windowWidth = width;
	windowHeight = height;
	
	glViewport(0,0,width,height);
	
	//printf("window resized to (%i, %i)\n", windowWidth, windowHeight);
	
	return;
}

const float points[] = {
	// position          // texture coords
	-1.0f,  1.0f, 0.0f,  0.0f,  1.0f,
	 1.0f, -1.0f, 0.0f,  1.0f,  0.0f,
	-1.0f, -1.0f, 0.0f,  0.0f,  0.0f,
		
	-1.0f,  1.0f, 0.0f,  0.0f,  1.0f,
	 1.0f,  1.0f, 0.0f,  1.0f,  1.0f,
	 1.0f, -1.0f, 0.0f,  1.0f,  0.0f,
};

// maybe not good to have like one vertex array object but idk it's literally just rotating scaling and translation a square
GLuint vertexArrayObject;

void initRenderer(){
	// set up vertex buffer object
	debugLog(LOG_NORMAL, "setting up vertex buffer object\n");
	GLuint vbo = 0;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, (sizeof(points)/sizeof(points[0])) * sizeof(float), points, GL_STATIC_DRAW);

	// set up vertex array object
	debugLog(LOG_NORMAL, "setting up vertex array object\n");
	glGenVertexArrays(1,&vertexArrayObject);
	glBindVertexArray(vertexArrayObject);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), NULL);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)(3*sizeof(float)));
	glEnableVertexAttribArray(1);
	
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
	
	return;
}


// size is between -1 and 1 like the shader or whatever, has to be converted from like world space or whatever into screen space
void drawFilledRect(rect drawnRect, colorRGBA color, float angle){
	glUniform1f(vertexAngleLocation, angle);
	glUniform4f(vertexRectLocation, drawnRect.x, drawnRect.y, drawnRect.w, drawnRect.h);
	glUniform4f(fragmentInputColorLocation, color.r, color.g, color.b, color.a);
	glUniform1ui(fragmentUseTextureLocation, GL_FALSE);
	
	glDrawArrays(GL_TRIANGLES, 0, sizeof(points)/sizeof(points[0]));
	
	return;
}

void drawTexture(rect drawnRect, rect textureRect, colorRGBA color, float angle, resource* texture){
	glUniform1f(vertexAngleLocation, angle);
	glUniform4f(vertexRectLocation, drawnRect.x, drawnRect.y, drawnRect.w, drawnRect.h);
	glUniform4f(fragmentTextureRectLocation, textureRect.x, textureRect.y, textureRect.w, textureRect.h);
	glUniform4f(fragmentInputColorLocation, color.r, color.g, color.b, color.a);
	glUniform1ui(fragmentUseTextureLocation, GL_TRUE);
	
	glBindTexture(GL_TEXTURE_2D, *(GLuint*)texture->pointer);
	glDrawArrays(GL_TRIANGLES, 0, sizeof(points)/sizeof(points[0]));
	
	return;
}
