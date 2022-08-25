#ifndef TEXTURE_LOADER_H
#define TEXTURE_LOADER_H

#include "resourceManager.h"

resource* loadTexture(const char* filename);
void destroyTexture(resource* res);

#endif
