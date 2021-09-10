#include "gameStates.h"

#include <stddef.h>

gameState* currentState = NULL;

bool running = true;

void initGameState(gameState* newState){
	if(currentState != NULL){
		(*(currentState->uninitState))();
	}
	
	(*(newState->initState))();
	currentState = newState;
}
