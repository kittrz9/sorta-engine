#ifndef PLAYER_H
#define PLAYER_H

#include <stdbool.h>

#include "resourceManager.h"
#include "vectors.h"
#include "entity.h"
#include "animation.h"

extern entity* player;

typedef struct {
	vec2f pos, size, vel;
	float jumpTimer;
	
	animationStruct* animation;
	
	bool facingLeft;
	bool moving;
} playerStruct;

entity* createPlayer(vec2f pos, vec2f size);

void drawPlayer(entity* ent);

void updatePlayerInAir(entity* ent, double deltaTime);
void updatePlayerOnGround(entity* ent, double deltaTime);

void destroyPlayer();

#endif
