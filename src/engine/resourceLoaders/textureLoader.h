#ifndef TEXTURE_LOADER_H
#define TEXTURE_LOADER_H

#include "resourceManager.h"
#include "vectors.h"
#include <GL/glew.h>

typedef struct {
	GLuint id;
	vec2f size;
} textureStruct;

resource* loadTexture(const char* filename);
void destroyTexture(resource* res);

#endif
