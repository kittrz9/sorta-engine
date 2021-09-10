#ifndef GAMESTATES_H
#define GAMESTATES_H

#include <stdbool.h>

typedef struct {
	void (*initState)(); // function pointer to function that initializes the game state
	void (*uninitState)(); // function pointer to function that uninitializes the game state 
	
	int (*stateLoop)(double); // function pointer called every game loop 
} gameState;

extern gameState* currentState;

extern bool running;

void initGameState(gameState* newState);

#endif
