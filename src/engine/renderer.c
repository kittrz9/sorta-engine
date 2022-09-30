#include "renderer.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "logging.h"
#include "controls.h"
#include "vectors.h"

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

const vertex points[] = {
	{.position={-1.0f, 1.0f}, .texCoords={0.0f,1.0f}},
	{.position={ 1.0f, 1.0f}, .texCoords={1.0f,1.0f}},
	{.position={ 1.0f,-1.0f}, .texCoords={1.0f,0.0f}},
	{.position={-1.0f,-1.0f}, .texCoords={0.0f,0.0f}},
};

// indices used when drawing triangles
const unsigned int baseTrisIndices[] = {
	0, 1, 2,
	2, 3, 0,
};

// this value is pretty much arbitrary
#define MAX_QUADS 4096
unsigned int trisIndices[MAX_QUADS * 6];

// indices used when drawing lines
const unsigned int linesIndices[] = {
	0, 1,
	1, 2,
	2, 3,
	3, 0,
};

vertexBuffer defaultVertexBuffer;
vertexBuffer textureVertexBuffer;
vertexBuffer textVertexBuffer;
GLuint vertexArrayObject;
GLuint elementBufferObject;

vertexBuffer* currentVertexBuffer = NULL;
void switchVertexBuffer(vertexBuffer* buf) {
	glBindBuffer(GL_ARRAY_BUFFER, buf->bufferID);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, position));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, texCoords));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, color));
	glEnableVertexAttribArray(2);

	currentVertexBuffer = buf;
}

void setupVertexBuffer(vertexBuffer* buf, unsigned int size, const void* data, GLenum mode) {
	glGenBuffers(1, &buf->bufferID);
	switchVertexBuffer(buf);
	glBufferData(GL_ARRAY_BUFFER, size, data, mode);
	buf->mode = mode;
}

void addQuadToVertexBuffer(vertexBuffer* buf, vertex* quad) {
	glBufferSubData(GL_ARRAY_BUFFER, buf->loadedVertices*sizeof(vertex), 4*sizeof(vertex), quad);

	vertex* test = malloc(sizeof(vertex) * 4);
	glGetBufferSubData(GL_ARRAY_BUFFER, 0, 4*sizeof(vertex), test);
	free(test);

	buf->loadedVertices += 4;
}

// draw a vertex buffer to the screen and clear the buffer (really only for dynamic buffers)
resource* currentTexture = NULL;
void flushVertexBuffer(vertexBuffer* buf) {
	if(buf->mode == GL_STATIC_DRAW) {
		return;
	}

	if(buf->loadedVertices == 0) { return; }

	if(buf == &textureVertexBuffer) {
		glBindTexture(GL_TEXTURE_2D, ((textureStruct*)currentTexture->pointer)->id);
	}
	glDrawElements(GL_TRIANGLES, buf->loadedVertices/4 * 6, GL_UNSIGNED_INT, 0);
	buf->loadedVertices = 0;

	return;
}
// called at the end of the frame
void flushAllVertexBuffers() {
	flushVertexBuffer(&textureVertexBuffer);
}

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

//	debugLog(LOG_NORMAL, "setting up text vertex object array\n");
//	glGenVertexArrays(1, &textVertexArray);
//	switchVertexBuffer(&textVertexBuffer);

	// set up vertex buffer object
	debugLog(LOG_NORMAL, "setting up vertex buffers\n");
	setupVertexBuffer(&defaultVertexBuffer, sizeof(points), points, GL_STATIC_DRAW);
	setupVertexBuffer(&textVertexBuffer, MAX_QUADS * 4 * sizeof(vertex), NULL, GL_DYNAMIC_DRAW);
	setupVertexBuffer(&textureVertexBuffer, MAX_QUADS * 4 * sizeof(vertex), NULL, GL_DYNAMIC_DRAW);
	
	// set up vertex array object
	debugLog(LOG_NORMAL, "setting up vertex array object\n");
	glGenVertexArrays(1,&vertexArrayObject);
	glBindVertexArray(vertexArrayObject);
	debugLog(LOG_SUCCESS, "successfully set up vertex array object\n");
	
	// set up element buffer object
	for(unsigned int i = 0; i < MAX_QUADS*6; ++i) {
		trisIndices[i] = baseTrisIndices[i%6] + (i/6)*4;
	}
	glGenBuffers(1, &elementBufferObject);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferObject);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(trisIndices), trisIndices, GL_STATIC_DRAW);
	debugLog(LOG_SUCCESS, "successfully set up element buffer object\n");


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
	glDeleteBuffers(1, &defaultVertexBuffer.bufferID);
	glDeleteBuffers(1, &textVertexBuffer.bufferID);

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

	if(currentVertexBuffer != NULL) {
		flushVertexBuffer(currentVertexBuffer);
	}
	
	currentShader = *(GLuint*)shaderRes->pointer;
	glUseProgram(currentShader);
	currentShaderRes = shaderRes;

	setShaderUniform2f("windowSize", windowWidth, windowHeight);
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

void drawTexture(rect drawnRect, rect textureRect, colorRGBA color, float angle, resource* textureRes) {
	if(textureRes->type != RES_TYPE_TEXTURE) {
		debugLog(LOG_ERROR, "resource \"%s\" is not a texture\n", textureRes->name);
		exit(1);
	}

	// really need to change this so I can use multiple textures per batch but for now this will work
	if(textureRes != currentTexture) {
		switchVertexBuffer(&textureVertexBuffer);
		currentTexture = textureRes;
		glBindTexture(GL_TEXTURE_2D, ((textureStruct*)textureRes->pointer)->id);
		flushVertexBuffer(&textureVertexBuffer);
	}

	switchVertexBuffer(&textureVertexBuffer);

	vertex quad[4];
	for(unsigned int i = 0; i < 4; ++i) {
		quad[i].position.x = (points[i].position.x * drawnRect.w) + drawnRect.x;
		quad[i].position.y = (points[i].position.y * drawnRect.h) + drawnRect.y;
		quad[i].texCoords.x = ((points[i].texCoords.x * textureRect.w) + textureRect.x) / ((textureStruct*)textureRes->pointer)->size.x;
		quad[i].texCoords.y = ((points[i].texCoords.y * textureRect.h) + textureRect.y) / ((textureStruct*)textureRes->pointer)->size.y;
		quad[i].color = color;
	}

	addQuadToVertexBuffer(&textureVertexBuffer, &quad[0]);

	if(textureVertexBuffer.loadedVertices >= MAX_QUADS * 4) {
		glBindTexture(GL_TEXTURE_2D, ((textureStruct*)textureRes->pointer)->id);
		flushVertexBuffer(&textureVertexBuffer);
	}
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
	glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), NULL);
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
	glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), NULL);
	return;
}

void drawText(resource* fontRes, char* text, float size, colorRGBA color, float x, float y) {
#define currentFont ((font*)fontRes->pointer)
	if(fontRes->type != RES_TYPE_FONT) {
		debugLog(LOG_ERROR, "resource \"%s\" is not a font\n", fontRes->name);
		exit(1);
	}
	switchVertexBuffer(&textVertexBuffer);
	unsigned int length = strlen(text);
	if(length > MAX_QUADS) {
		printf("string is too long to be printed (%i)\n", length);
	}
	unsigned int verticesLength = 0;
	for(unsigned int i = 0; i < length; ++i) {
		if(currentFont->chars[(unsigned int)text[i]].loaded && text[i] != ' ') {
			verticesLength++;
		}
	}

	vertex* textVertices = malloc(4*sizeof(vertex) * verticesLength);
	
	float xPos = x;
	float yPos = y;
	unsigned int j = 0;
	for(unsigned int i = 0; i < length; ++i) {
		fontChar currentCharData = currentFont->chars[(unsigned int)text[i]];
		if(text[i] == '\n') {
			yPos += size*1.5; // hard coded value that probably wont work for other fonts
			xPos = x;
			continue;
		}
		if(!currentCharData.loaded) {
			continue;
		}
		if(text[i] != ' ') {
			// this is a mess
			float charAtlasX = currentCharData.atlasLeft;
			float charAtlasY = currentFont->textureHeight - currentCharData.atlasTop;
			float charAtlasW = currentCharData.atlasRight - charAtlasX;
			float charAtlasH = currentCharData.atlasTop - currentCharData.atlasBottom;

			float charX = currentCharData.left;
			float charY = currentCharData.top + currentCharData.bottom;
			float charW = currentCharData.right - charX;
			float charH = currentCharData.top - currentCharData.bottom;

			float drawnX = (xPos*2-windowWidth)/(float)windowWidth + (1.0f - currentCharData.right) * size/windowWidth;
			float drawnY = (windowHeight-yPos*2 - size/2)/(float)windowHeight - (1.0f - charY) * (size/windowHeight);
			float drawnW = (charW * size)/windowWidth;
			float drawnH = (charH * size)/windowHeight;

			for(unsigned int k = 0; k < 4; ++k) {
				textVertices[(j*4)+k].position.x = (points[k].position.x * drawnW) + drawnX;
				textVertices[(j*4)+k].position.y = (points[k].position.y * drawnH) + drawnY;
				textVertices[(j*4)+k].texCoords.x = charAtlasX + (points[k].texCoords.x * charAtlasW);
				textVertices[(j*4)+k].texCoords.y = charAtlasY + ((1.0f -points[k].texCoords.y) * charAtlasH); // characters get rendered upside down unless I do this
			}

			++j;
		}
		xPos += currentCharData.advance * size;
	}

	setShaderUniform4f("inputColor", color.r, color.g, color.b, color.a);
	
	glBindTexture(GL_TEXTURE_2D, *currentFont->texture);
	glBufferSubData(GL_ARRAY_BUFFER, 0, 4*verticesLength*sizeof(vertex), textVertices);
	glDrawElements(GL_TRIANGLES, verticesLength*6, GL_UNSIGNED_INT, 0);

	switchVertexBuffer(&defaultVertexBuffer);
	free(textVertices);

	return;
}
