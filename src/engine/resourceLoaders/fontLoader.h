#ifndef FONT_LOADER_H
#define FONT_LOADER_H

#include <stdbool.h>

#include "glad/gl.h"

#include "resourceManager.h"

typedef struct {
	bool loaded;
	float advance;
	float left;
	float bottom;
	float right;
	float top;
	float atlasLeft;
	float atlasBottom;
	float atlasRight;
	float atlasTop;
} fontChar;

typedef struct {
	fontChar chars[256];
	GLuint* texture;
	unsigned int textureWidth;
	unsigned int textureHeight;
} font;

resource* loadFont(const char* name, const char* fontFilename, const char* fontDataFilename);
void destroyFont(resource* res);

#endif
