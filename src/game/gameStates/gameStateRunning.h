#ifndef GAMESTATE_RUNNING_H
#define GAMESTATE_RUNNING_H

#include "game/gameStates.h"

void initGameStateRunning(void);
void uninitGameStateRunning(void);
int runGameStateRunning(double deltaTime);

static const gameState gameStateRunning = {
	.initState = initGameStateRunning,
	.uninitState = uninitGameStateRunning,
	.stateLoop = runGameStateRunning,
};

#endif
