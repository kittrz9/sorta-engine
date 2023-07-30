#include "gameStates/gameStateRunning.h" 

#include <stdio.h>
#include <stdlib.h>

#include "entity.h"
#include "player.h"
#include "controls.h"
#include "audio.h"
#include "files.h"

#include "renderer.h"

struct {
	resource* defaultFont;
	resource* fontShader;
	resource* fontShader2;
	resource* playerShader;
	resource* backgroundShader;
	gameFile testFile;
	//resource* testSample;
} gsRunningData; // to avoid scope shenanigans with other game states that might use names like this (though this leads to long struct stuff any time you need this data)


void initGameStateRunning(void){
	player = createPlayer((vec2f){0,0}, (vec2f){175,175});

	gsRunningData.defaultFont = loadFont("Terminus", "fonts/Terminus.png", "fonts/Terminus.csv.bz2");
	gsRunningData.fontShader = loadShader("fontShader", "shaders/fontVertexShader.glsl", "shaders/fontFragmentShader.glsl");
	gsRunningData.playerShader = loadShader("playerShader", "shaders/vertexShader.glsl", "shaders/fragmentShader2.glsl");
	gsRunningData.fontShader2 = loadShader("fontShader2", "shaders/fontVertexShader2.glsl", "shaders/fontFragmentShader.glsl");

	gsRunningData.backgroundShader = loadShader("backgroundShader", "shaders/vertexShader.glsl", "shaders/backgroundFrag.glsl");

	gsRunningData.testFile = readGameFile("test.txt", true);


	// I can't think of a good sample to use that wouldn't risk copyright stuff so this isn't included in the git repo and commented out
	//gsRunningData.testSample = loadWav("sounds/24_Moonsong.wav");

	//playSample(gsRunningData.testSample, 0.1, 1.0);
	
	return;
}

void uninitGameStateRunning(void){
	free(gsRunningData.testFile.buffer);
	removeEntity(player);

	return;
}

int runGameStateRunning(double deltaTime){
	useShader(gsRunningData.backgroundShader);
	static float backgroundTimer = 0.0f;
	setShaderUniform1f("time", backgroundTimer);
	drawFilledRect((rect){0,0,windowWidth,windowHeight}, (colorRGBA){1.0f, 1.0f, 1.0f, 1.0f}, 0.0f);

	backgroundTimer += deltaTime;
	if(backgroundTimer >= 3.1415926535 * 2) { backgroundTimer = 0.0f; }

	useShader(gsRunningData.playerShader);
	processEntities(deltaTime);
	
	if(keys[KEY_EXIT].held){
		running = false;
	}
	
	char fpsStr[32];
	sprintf(fpsStr, "fps: %.2f", 1/deltaTime);
	useShader(gsRunningData.fontShader);
	drawText(gsRunningData.defaultFont, fpsStr, 25.0, (colorRGBA){0.0f,0.0f,1.0f,1.0f}, 0, 0, TEXT_ALIGN_LEFT);

	static float fontTime = 0.0f;
	useShader(gsRunningData.fontShader2);
	setShaderUniform1f("time", fontTime);
	drawText(gsRunningData.defaultFont, gsRunningData.testFile.buffer, 100.0, (colorRGBA){1.0f, 0.0f, 1.0f, 1.0f}, windowWidth/2, windowHeight/2 - 50, TEXT_ALIGN_CENTER);
	fontTime += deltaTime * 0.2;
	if(fontTime >= 1.0f) { fontTime = 0.0f; }
	
	return 0;
}
