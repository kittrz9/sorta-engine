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
resource* fontShader2;
resource* playerShader;
resource* backgroundShader;
char* fpsStr;

void initGameStateRunning(){
	player = createPlayer((vec2f){0,0}, (vec2f){175,175});

	defaultFont = loadFont("Terminus", "fonts/Terminus.png", "fonts/Terminus.csv");
	fontShader = loadShader("fontShader", "shaders/fontVertexShader.glsl", "shaders/fontFragmentShader.glsl");
	playerShader = loadShader("playerShader", "shaders/vertexShader.glsl", "shaders/fragmentShader2.glsl");
	fontShader2 = loadShader("fontShader2", "shaders/fontVertexShader2.glsl", "shaders/fontFragmentShader.glsl");

	backgroundShader = loadShader("backgroundShader", "shaders/vertexShader2.glsl", "shaders/backgroundFrag.glsl");
	
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
	useShader(backgroundShader);
	static float backgroundTimer = 0.0f;
	setShaderUniform1f("time", backgroundTimer);
	// just drawing this quad halves the framerate lmao, probably because it's setting a ton of uniforms since this function still uses the old method of setting up vertices
	drawFilledRect((rect){0,0,windowWidth,windowHeight}, (colorRGBA){1.0f, 1.0f, 1.0f, 1.0f}, 0.0f);
	backgroundTimer += deltaTime;
	if(backgroundTimer >= 3.1415926535 * 2) { backgroundTimer = 0.0f; }

	useShader(playerShader);
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
	drawText(defaultFont, fpsStr, 25.0, (colorRGBA){0.0f,0.0f,1.0f,1.0f}, 0, 0);

	static float fontTime = 0.0f;
	useShader(fontShader2);
	setShaderUniform1f("time", fontTime);
	drawText(defaultFont, "balls", 100.0, (colorRGBA){1.0f, 0.0f, 1.0f, 1.0f}, windowWidth/2 - 130, windowHeight/2 - 50);
	fontTime += deltaTime * 0.2;
	if(fontTime >= 1.0f) { fontTime = 0.0f; }

	
	return 0;
}
