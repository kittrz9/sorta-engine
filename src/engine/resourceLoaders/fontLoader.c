#include "fontLoader.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "stb_image/stb_image.h"

#include "logging.h"
#include "resourceManager.h"
#include "files.h"

resource* loadFont(const char* name, const char* fontFilename, const char* fontDataFilename) {
	resource* tempRes = checkIfAlreadyLoaded(name);
	if(tempRes != NULL) {
		return tempRes;
	}

	resource* res = malloc(sizeof(resource));

	font* resFont = malloc(sizeof(font));

	// load font msdf texture
	debugLog(LOG_NORMAL, "loading font texture \"%s\"\n", fontFilename);
	gameFile fontTextureFile = readGameFile(fontFilename, false);

	if(fontTextureFile.buffer == NULL) {
		exit(1);
	}

	int width, height, nrChannels;
	GLuint* texture = malloc(sizeof(GLuint));

	glGenTextures(1, texture);
	glBindTexture(GL_TEXTURE_2D, *texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	unsigned char* data = stbi_load_from_memory(fontTextureFile.buffer, fontTextureFile.size, &width, &height, &nrChannels, 4);
	if(data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		resFont->texture = texture;
		resFont->textureWidth = width;
		resFont->textureHeight = height;
		free(fontTextureFile.buffer);
	} else {
		debugLog(LOG_ERROR, "couldn't load font \"%s\" with msdf \"%s\"", name, fontFilename);
		free(texture);
		free(data);
		free(fontTextureFile.buffer);
		return NULL;
	}
	stbi_image_free(data);

	// load msdf font data
	char* fontData;
	
	debugLog(LOG_NORMAL, "loading font data \"%s\"\n", fontDataFilename);
	

	// compressing the csv file with gzip made it go from like 10kb to 1kb
	gameFile fontDataFile = readGameFile(fontDataFilename, true);
	fontData = fontDataFile.buffer;
	if(fontData == NULL) {
		exit(1);
	}
	
	// parse font data
	unsigned int lineCount = 0;
	unsigned int maxLineLength = 0;
	unsigned int currentLineLength = 0;
	debugLog(LOG_NORMAL, "counting lines\n");
	for(char* c = fontData; ; ++c) {
		if(*c == '\n') {
			if(currentLineLength > maxLineLength) {
				maxLineLength = currentLineLength;
			}
			currentLineLength = 0;
			++lineCount;
			continue;
		}
		// should probably not have this repeated code
		if(*c == '\0') {
			if(currentLineLength > maxLineLength) {
				maxLineLength = currentLineLength;
			}
			break;
		}
		++currentLineLength;
	}
	char** lines = malloc(sizeof(char*) * lineCount);
	for(unsigned int i = 0; i < lineCount; ++i) {
		// this is probably dumb and allocates a ton of memory but should be fine if I just free that memory, hopefully
		lines[i] = malloc(sizeof(char) * maxLineLength + 1);
	}

	debugLog(LOG_NORMAL, "reading data from lines\n");
	char* c = fontData;
	unsigned int i = 0;
	unsigned int j = 0;
	while(*c != '\0') {
		if(*c == '\n') {
			lines[i][j] = '\0';
			++i;
			j = 0;
			++c;
			continue;
		}
		lines[i][j] = *c;
		++j;
		++c;
	}

	debugLog(LOG_NORMAL, "parsing lines\n");
	const char* delim = ",";
	char* str = malloc(sizeof(char) * 32); // probably more than necessary
	char charIndex;
	for(i = 0; i < lineCount-1; ++i) {
		// get char value
		strcpy(str, strtok(lines[i], delim));
		if(str == NULL) {
			debugLog(LOG_ERROR, "failed to parse char value on line %i\n", i);
			break;
		}
		charIndex = atoi(str);
		// bunch of repeated code, should probably be changed
#define GET_FONT_DATA(TYPE) \
strcpy(str, strtok(NULL, delim)); \
if(str == NULL) { \
	debugLog(LOG_ERROR, "failed to parse value %i on line %i\n", j, i); \
	break; \
} \
resFont->chars[(unsigned int)charIndex].TYPE = atof(str); /* have to cast to unsigned int to avoid warning */

		GET_FONT_DATA(advance);
		GET_FONT_DATA(left);
		GET_FONT_DATA(bottom);
		GET_FONT_DATA(right);
		GET_FONT_DATA(top);
		GET_FONT_DATA(atlasLeft);
		GET_FONT_DATA(atlasBottom);
		GET_FONT_DATA(atlasRight);
		GET_FONT_DATA(atlasTop);

		resFont->chars[(unsigned int)charIndex].loaded = true;
	}
	free(str);
	for(i = 0; i < lineCount; ++i) {
		free(lines[i]);
	}
	free(fontData);
	free(lines);

	// return resource
	res->pointer = resFont;

	addResourceToList(RES_TYPE_FONT, name, res);
	return res;
}

#define resFont ((font*)res->pointer)
void destroyFont(resource* res) {
	free(resFont->texture);
	free(resFont);
}
