#include "game/gameStates.h"

#include <stddef.h>

gameState* currentState = NULL;

bool running = true;

void initGameState(const gameState* newState){
	if(currentState != NULL){
		(*(currentState->uninitState))();
	}
	
	(*(newState->initState))();
	// casting this to a pointer without const for some reason fixes the warning about disregarding the const qualifier
	currentState = (gameState*)newState;
}
