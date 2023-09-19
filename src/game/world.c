#include "game/world.h"

vec2f worldToScreenSpace(vec2f inputVector){
	return (vec2f){ (inputVector.x/worldSize.x)*2-1, (inputVector.y/worldSize.y)*2-1};
}

vec2f screenToWorldSpace(vec2f inputVector){
	return (vec2f){ (((inputVector.x+1)/2)*worldSize.x), (((inputVector.y+1)/2)*worldSize.y) };
}

