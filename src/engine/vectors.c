#include "engine/vectors.h"

#include <math.h>
 
vec2f scaleVec2f(vec2f inVector, vec2f scaleVector){
	return (vec2f){inVector.x * scaleVector.x, inVector.y * scaleVector.y};
}
vec2f rotateVec2f(vec2f inVector, float angle){
	return (vec2f){(cos(angle) * inVector.x) + (sin(angle) * inVector.y), (cos(angle) * inVector.y) - (sin(angle) * inVector.x)};
}
vec2f translateVec2f(vec2f inVector, vec2f translationVector){
	return (vec2f){inVector.x + translationVector.x, inVector.y + translationVector.y};
}
