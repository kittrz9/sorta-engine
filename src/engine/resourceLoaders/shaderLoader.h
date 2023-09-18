#ifndef SHADER_LOADER_H
#define SHADER_LOADER_H

#include "engine/resourceManager.h"

// might be stupid to have a separate name parameter since the texture loading function doesn't need one but whatever this is fine for now
resource* loadShader(const char* name, const char* vertexShaderPath, const char* fragmentShaderPath);
void destroyShader(resource* res);

#endif
