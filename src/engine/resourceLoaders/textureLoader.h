#ifndef TEXTURE_LOADER_H
#define TEXTURE_LOADER_H

#include "engine/resourceManager.h"
#include "engine/vectors.h"

#include "glad/gl.h"

typedef struct {
	GLuint id;
	vec2f size;
} textureStruct;

resource* loadTexture(const char* filename);
void destroyTexture(resource* res);

#endif
