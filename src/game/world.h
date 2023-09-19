#ifndef WORLD_H
#define WORLD_H

#include "engine/vectors.h"

static const vec2f worldSize = {1333.3f, 1000.0f};
vec2f worldToScreenSpace(vec2f inputVector);
vec2f screenToWorldSpace(vec2f inputVector);

#endif
