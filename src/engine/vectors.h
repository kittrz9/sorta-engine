#ifndef VECTORS_H
#define VECTORS_H

typedef struct {
	float x,y;
} vec2f;
typedef struct {
	float x,y,z;
} vec3f;
typedef struct {
	float x,y,z,w;
} vec4f;

vec2f scaleVec2f(vec2f inVector, vec2f scaleVector);
vec2f rotateVec2f(vec2f inVector, float angle);
vec2f translateVec2f(vec2f inVector, vec2f translationVector);

#endif

