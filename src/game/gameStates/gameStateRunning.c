#include "gameStates/gameStateRunning.h" 

#include <stdio.h>
#include <stdlib.h>

#include "entity.h"
#include "player.h"
#include "controls.h"

#include "renderer.h"

entity* player;
resource* defaultFont;
resource* fontShader;
resource* defaultShader2;

void initGameStateRunning(){
	player = createPlayer((vec2f){0,0}, (vec2f){175,175});

	defaultFont = loadFont("Terminus", "fonts/Terminus.png", "fonts/Terminus.csv");
	fontShader = loadShader("fontShader", "shaders/fontVertexShader.glsl", "shaders/fontFragmentShader.glsl");
	defaultShader2 = loadShader("defaultShader2", "shaders/vertexShader.glsl", "shaders/fragmentShader2.glsl");
	
	changeClearScreenColor((colorRGBA){0.5f, 0.5f, 0.5f, 1.0f});
	
	return;
}

void uninitGameStateRunning(){
	removeEntity(player);
	
	return;
}

int runGameStateRunning(double deltaTime){
	useShader(defaultShader2);
	for(entListCurrent = entListHead; entListCurrent != NULL; entListCurrent = entListCurrent->next){
		// call the entities draw and update functions
		(*entListCurrent->ent->draw)(entListCurrent->ent);
		(*entListCurrent->ent->update)(entListCurrent->ent, deltaTime);
	}
	
	if(keys[EXIT].held){
		running = false;
	}
	
	char* str = malloc(sizeof(char) * 32);
	sprintf(str, "fps: %.2f", 1/deltaTime);
	useShader(fontShader);
	drawText(defaultFont, str, 50.0, (colorRGBA){0.0f,0.0f,1.0f,1.0f}, 0, 50);
	
	return 0;
}
