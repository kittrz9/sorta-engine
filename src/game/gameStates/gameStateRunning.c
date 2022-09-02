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
char* fpsStr;

void initGameStateRunning(){
	player = createPlayer((vec2f){0,0}, (vec2f){175,175});

	defaultFont = loadFont("Terminus", "fonts/Terminus.png", "fonts/Terminus.csv");
	fontShader = loadShader("fontShader", "shaders/fontVertexShader.glsl", "shaders/fontFragmentShader.glsl");
	defaultShader2 = loadShader("defaultShader2", "shaders/vertexShader.glsl", "shaders/fragmentShader2.glsl");
	
	changeClearScreenColor((colorRGBA){0.5f, 0.5f, 0.5f, 1.0f});

	fpsStr = malloc(sizeof(char) * 32);
	
	return;
}

void uninitGameStateRunning(){
	removeEntity(player);
	
	free(fpsStr);

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
	
	sprintf(fpsStr, "fps: %.2f", 1/deltaTime);
	useShader(fontShader);
	drawText(defaultFont, fpsStr, 50.0, (colorRGBA){0.0f,0.0f,1.0f,1.0f}, 0, 20); // font renders too high up right now, temporary fix

	drawText(defaultFont, "placeholder text placeholder text\nplaceholder text placeholder text\nplaceholder text placeholder text\nplaceholder text placeholder text\nplaceholder text placeholder text\nplaceholder text placeholder text\nplaceholder text placeholder text\nplaceholder text placeholder text\nplaceholder text placeholder text\nplaceholder text placeholder text\nplaceholder text placeholder text\n", 30, (colorRGBA){1.0f, 0.0f, 1.0f, 1.0f}, 0, 200);

	
	return 0;
}
