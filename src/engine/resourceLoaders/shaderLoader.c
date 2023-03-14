#include "shaderLoader.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <GL/glew.h>

#include "logging.h"
#include "resourceManager.h"

GLuint readAndCompileShader(const char* shaderFilePath, GLenum shaderType){
	FILE* filePointer;
	unsigned int fileSize;
	char* shaderContents;
	GLint  shaderCompiled = 1;
	GLuint shader;

	char* shaderTypeString;
	if(shaderType == GL_VERTEX_SHADER) {
		shaderTypeString = "vertex";
	} else if(shaderType == GL_FRAGMENT_SHADER) {
		shaderTypeString = "fragment";
	} else {
		debugLog(LOG_ERROR, "unknown shader type");
		return 0;
	}
	
	// read shader into string
	filePointer = fopen(shaderFilePath, "r");
	if(!filePointer) {
		debugLog(LOG_ERROR, "could not open shader \"%s\"", shaderFilePath);
		return 0;
	}
	fseek(filePointer, 0, SEEK_END);
	fileSize = ftell(filePointer);
	rewind(filePointer);
	// string needs to be 1 char longer to have a null character to terminate the string
	shaderContents = malloc((fileSize+1) * sizeof(char));
	fread(shaderContents, fileSize, 1, filePointer);
	shaderContents[fileSize] = '\0';
	debugLog(LOG_NORMAL, "compiling %s shader \"%s\", size %i\n", shaderTypeString, shaderFilePath, fileSize);
	
	// compile shader
	shader = glCreateShader(shaderType);
	glShaderSource(shader, 1, (const GLchar**)&shaderContents, NULL);
	glCompileShader(shader);
	
	// check if shader compiled successfully 
	glGetShaderiv(shader, GL_COMPILE_STATUS, &shaderCompiled);
	if(shaderCompiled == GL_FALSE){
		GLsizei logLength = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
		GLchar* message = malloc(sizeof(GLchar) * logLength);
		glGetShaderInfoLog(shader, logLength, NULL, message);
		debugLog(LOG_ERROR, "could not compile %s shader \"%s\": %s\n", shaderTypeString, shaderFilePath, message);
		free(message);
		return 0;
	}
	
	debugLog(LOG_SUCCESS, "%s shader \"%s\" compiled successfully\n", shaderTypeString, shaderFilePath);
	fclose(filePointer);
	free(shaderContents);
	
	return shader;
}

resource* loadShader(const char* name, const char* vertexShaderPath, const char* fragmentShaderPath) {
	resource* tempRes = checkIfAlreadyLoaded(name);
	if(tempRes != NULL) {
		return tempRes;
	}

	resource* res = malloc(sizeof(resource));

	// compile vertex and fragment shader
	char* fullResourcePath = malloc(resDirStrLen + strlen(vertexShaderPath) + 1);
	sprintf(fullResourcePath, "%s%s", resourceDir, vertexShaderPath);

	GLuint vertexShader = readAndCompileShader(fullResourcePath, GL_VERTEX_SHADER);
	free(fullResourcePath);
	fullResourcePath = malloc(resDirStrLen + strlen(fragmentShaderPath) + 1);
	sprintf(fullResourcePath, "%s%s", resourceDir, fragmentShaderPath);

	GLuint fragmentShader = readAndCompileShader(fullResourcePath, GL_FRAGMENT_SHADER);
	free(fullResourcePath);
	
	// link them
	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	GLint shaderCompiled = 1;
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &shaderCompiled);
	if(!shaderCompiled) {
		GLsizei logLength = 0;
		glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &logLength);
		GLchar* message = malloc(sizeof(GLchar) * logLength);
		glGetProgramInfoLog(shaderProgram, logLength, NULL, message);
		debugLog(LOG_ERROR, "could not link shader \"%s\": %s\n", name, message);
		free(message);
		return NULL;
	}

	
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	//res->pointer = &shaderProgram;
	res->pointer = malloc(sizeof(GLuint));
	*(GLuint*)res->pointer = shaderProgram;

	addResourceToList(RES_TYPE_SHADER, name, res);
	
	return res;
}

void destroyShader(resource* res) {
	glDeleteProgram(*(GLuint*)res->pointer);
	free(res->pointer);

	return;
}
