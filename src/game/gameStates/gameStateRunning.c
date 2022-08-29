#include "gameStates/gameStateRunning.h" 

#include <stdio.h>

#include "entity.h"
#include "player.h"
#include "controls.h"

#include "renderer.h"

entity* player;

void initGameStateRunning(){
	player = createPlayer((vec2f){0,0}, (vec2f){175,175});
	
	changeClearScreenColor((colorRGBA){0.5f, 0.5f, 0.5f, 1.0f});
	
	return;
}

void uninitGameStateRunning(){
	removeEntity(player);
	
	return;
}

int runGameStateRunning(double deltaTime){
	for(entListCurrent = entListHead; entListCurrent != NULL; entListCurrent = entListCurrent->next){
		// call the entities draw and update functions
		(*entListCurrent->ent->draw)(entListCurrent->ent);
		(*entListCurrent->ent->update)(entListCurrent->ent, deltaTime);
	}
	
	if(keys[EXIT].held){
		running = false;
	}
	
	printf("fps: %.2f\r", 1/deltaTime);
	
	return 0;
}
