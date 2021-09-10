#ifndef GAMESTATE_RUNNING_H
#define GAMESTATE_RUNNING_H

#include "gameStates.h"

void initGameStateRunning();
void uninitGameStateRunning();
int runGameStateRunning(double deltaTime);

static gameState gameStateRunning = {
	.initState = initGameStateRunning,
	.uninitState = uninitGameStateRunning,
	.stateLoop = runGameStateRunning,
};

#endif
