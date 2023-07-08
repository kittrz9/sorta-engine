#ifndef ANIMATION_H
#define ANIMATION_H

#include <stdint.h>

#include "resourceManager.h"
#include "renderer.h"

typedef struct {
	rect textureRect;
	float delay; // time until next frame
} animationFrame;

typedef struct {
	resource* texture;
	animationFrame* frames;
	uint16_t length; // length in frames
	uint16_t index; // what frame the animation is at now
	float timer; // to check if enough time has passed to go to the next frame
	
	animationFrame* nextAnim; // the animation to be played after this one is finished (repeats if NULL)
	uint16_t nextAnimSize; // length of next animation in frames
} animationStruct;

animationStruct* createAnimationStruct(resource* texture, animationFrame* anim, uint16_t animSize, animationFrame* nextAnim, uint16_t nextAnimSize);
void setAnimation(animationStruct* anim, animationFrame* frames, uint16_t length);
void updateAnimation(animationStruct* anim, double deltaTime);

typedef enum {
	ANIM_FLIP_X = 1 << 0,
	ANIM_FLIP_Y = 1 << 1,
} flipAnim;

void drawAnimation(animationStruct* anim, rect drawnRect, flipAnim flip, colorRGBA color);
#endif
