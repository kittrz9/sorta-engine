#include "shader.h"

#include <stdio.h>
#include <stdlib.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

GLuint readAndCompileShader(const char* shaderFilePath, GLenum shaderType){
	FILE* filePointer;
	unsigned int fileSize;
	char* shaderContents;
	GLint  shaderCompiled = 1;
	GLuint shader;
	
	// read shader into string
	filePointer = fopen(shaderFilePath, "r");
	if(!filePointer) {
		printf("could not open shader");
		return 0;
	}
	fseek(filePointer, 0, SEEK_END);
	fileSize = ftell(filePointer);
	rewind(filePointer);
	// string needs to be 1 char longer to have a null character to terminate the string
	shaderContents = malloc((fileSize+1) * sizeof(char));
	fread(shaderContents, fileSize, 1, filePointer);
	shaderContents[fileSize] = '\0';
	printf("shader \"%s\", size %i\n\"%s\"\n", shaderFilePath, fileSize, shaderContents);
	
	// compile shader
	shader = glCreateShader(shaderType);
	glShaderSource(shader, 1, (const GLchar**)&shaderContents, NULL);
	glCompileShader(shader);
	
	// check if shader compiled successfully 
	glGetShaderiv(shader, GL_COMPILE_STATUS, &shaderCompiled);
	if(shaderCompiled == GL_FALSE){
		printf("could not compile shader \"%s\"\n", shaderFilePath);
		
		GLsizei logLength = 0;
		GLchar message[1024];
		glGetShaderInfoLog(shader, 1024, &logLength, message);
		printf("%s\n", message);
		
		return 0;
	}
	
	printf("shader \"%s\" compiled successfully\n", shaderFilePath);
	fclose(filePointer);
	free(shaderContents);
	
	return shader;
}


GLuint createShader(const char* vertexShaderFilePath, const char* fragmentShaderFilePath){
	// compile vertex and fragment shader
	GLuint vertexShader = readAndCompileShader(vertexShaderFilePath, GL_VERTEX_SHADER);
	GLuint fragmentShader = readAndCompileShader(fragmentShaderFilePath, GL_FRAGMENT_SHADER);
	
	// link them
	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	
	return shaderProgram;
}
