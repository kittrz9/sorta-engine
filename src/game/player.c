#include "player.h"

#include <stdio.h>
#include <stdlib.h>

#include "renderer.h"
#include "controls.h"
#include "vectors.h"

#include "audio.h"

#define playerObj ((playerStruct*)(ent->object))

const vec2f worldSize = {1333.3f, 1000.0f};

vec2f worldToScreenSpace(vec2f inputVector){
	return (vec2f){ (inputVector.x/worldSize.x)*2-1, (inputVector.y/worldSize.y)*2-1};
}

animationFrame runAnimation[] = {
	{
		.textureRect = {
			.x = 0,
			.y = 0,
			.w = 326,
			.h = 379,
		},
		.delay = 0.1f,
	},
	{
		.textureRect = {
			.x = 326,
			.y = 0,
			.w = 326,
			.h = 379,
		},
		.delay = 0.1f,
	},
};

animationFrame idleAnimation[] = {
	{
		.textureRect = {
			.x = 0,
			.y = 0,
			.w = 326,
			.h = 379,
		},
		.delay = -1.0f, // repeats
	},
};

entity* createPlayer(vec2f pos, vec2f size){
	entity* ent = malloc(sizeof(entity));
	ent->object = malloc(sizeof(playerStruct));
	
	playerObj->pos = pos;
	playerObj->size = size;
	playerObj->vel = (vec2f){0,0};
	
	playerObj->jumpTimer = 0.0f;
	
	playerObj->animation = malloc(sizeof(animationStruct));
	playerObj->animation->texture = loadResource(RES_TYPE_TEXTURE, "res/test.pngasdf");
	playerObj->animation->timer = 0.0f;
	playerObj->animation->index = 0;
	playerObj->animation->frames = idleAnimation;
	playerObj->animation->length = sizeof(idleAnimation)/sizeof(animationFrame);
	playerObj->animation->nextAnim = NULL;
	
	playerObj->moving = false;
	playerObj->facingLeft = false;
	
	ent->draw = drawPlayer;
	ent->update = updatePlayerInAir;
	
	ent->destructor = destroyPlayer;
	
	pushToEntityList(ent);
	
	return ent;
}

void destroyPlayer(entity* ent){
	free(playerObj->animation);
	
	return;
}

void drawPlayer(entity* ent){
	// this is a hacked together mess that barely works
	vec2f drawnPos = worldToScreenSpace(playerObj->pos);
	vec2f drawnSize = worldToScreenSpace(playerObj->size);
	drawnSize = (vec2f){(drawnSize.x+1)/2, ((drawnSize.y+1)/2)};
	rect drawnRect = {
		drawnPos.x + drawnSize.x,
		-drawnPos.y - drawnSize.y,
		drawnSize.x * (playerObj->facingLeft ? -1 : 1),
		-drawnSize.y,
	};
	
	drawTexture(drawnRect, playerObj->animation->frames[playerObj->animation->index].textureRect, (colorRGBA){1.0f,1.0f,1.0f,1.0f}, 0.0f, playerObj->animation->texture);
	
	return;
}

void playerBoundaryCheck(entity* ent){
	if(playerObj->pos.x < 0) {playerObj->pos.x = 0;}
	if(playerObj->pos.y < 0) {playerObj->pos.y = 0;}
	if(playerObj->pos.x > worldSize.x - playerObj->size.x) {playerObj->pos.x =  worldSize.x - playerObj->size.x;}
	if(playerObj->pos.y > worldSize.y - playerObj->size.y) {playerObj->pos.y =  worldSize.y - playerObj->size.y;}
	
	return;
}

synthInstrument jumpSndInstr = {
	.envelope = {
		.attack = 0.01,
		.decay = 0.05f,
		.sustain = 0.8f,
		
		.release = 0.2f,
	},
	.synth = synthTriangle,
};
synthData jumpSndData = {
	.startFreq = 220.0,
	.endFreq = 440.0,
	.volume = 1.0,
	.length = 0.025f,
	.channel = -1,
	.instrument = &jumpSndInstr,
};

synthInstrument stepSndInstr = {
	.envelope = {
		.attack = 0.01,
		.decay = 0.05f,
		.sustain = 0.8f,
		
		.release = 0.1f,
	},
	.synth = synthTriangle,
};
synthData stepSndData = {
	.startFreq = 200.0,
	.endFreq = 20.0,
	.volume = 1.0,
	.length = 0.025f,
	.channel = -1,
	.instrument = &stepSndInstr,
};

// I hate how because this is in like 1 second instead of 1 millisecond they're all massive values
#define MAX_RUN_SPEED 1000.0f
#define PLAYER_RUN_ACCEL 8000.0f
#define GRAVITY 15000.0f
#define JUMP_FORCE -3000.0f
#define FRICTION 6000.0f

void commonPhysicsCheck(entity* ent, double deltaTime){
	playerObj->pos = translateVec2f(playerObj->pos, (vec2f){playerObj->vel.x * deltaTime, playerObj->vel.y * deltaTime});
	
	playerBoundaryCheck(ent);
	
	if(keys[LEFT].held) {
		playerObj->facingLeft = true;
		playerObj->moving = true;
		if(playerObj->vel.x > -MAX_RUN_SPEED){
			playerObj->vel.x -= PLAYER_RUN_ACCEL * deltaTime;
		} else {
			playerObj->vel.x = -MAX_RUN_SPEED;
		}
	} else if(keys[RIGHT].held) {
		playerObj->facingLeft = false;
		playerObj->moving = true;
		if(playerObj->vel.x < MAX_RUN_SPEED){
			playerObj->vel.x += PLAYER_RUN_ACCEL * deltaTime;
		} else {
			playerObj->vel.x = MAX_RUN_SPEED;
		}
	} else {
		if(abs(playerObj->vel.x) >= 0.001f) {
			playerObj->vel.x += FRICTION * (playerObj->vel.x > 0 ? -1 : 1) * deltaTime;
		} else {
			playerObj->vel.x = 0.0f;
		}
		playerObj->moving = false;
	}
}

void updatePlayerOnGround(entity* ent, double deltaTime){
	commonPhysicsCheck(ent, deltaTime);
	
	if(keys[UP].pressedTimer > 0.0f){
		playSynth(&jumpSndData);
		playerObj->vel.y = JUMP_FORCE;
		ent->update = updatePlayerInAir;
	}
	
	if(playerObj->animation->frames != runAnimation && playerObj->moving){
		setAnimation(playerObj->animation, runAnimation, sizeof(runAnimation)/sizeof(animationFrame));
	} else if(playerObj->animation->frames != idleAnimation && !playerObj->moving){
		setAnimation(playerObj->animation, idleAnimation, sizeof(idleAnimation)/sizeof(animationFrame));
	}
	updateAnimation(playerObj->animation, deltaTime);
	if(playerObj->animation->frames == runAnimation && playerObj->animation->index == 0 && playerObj->animation->timer == 0.0f){
		playSynth(&stepSndData);
	}
	
	playerBoundaryCheck(ent);
}
void updatePlayerInAir(entity* ent, double deltaTime){
	commonPhysicsCheck(ent, deltaTime);
	
	if(playerObj->jumpTimer > 0.0f){
		playerObj->jumpTimer -= deltaTime;
	}
	
	if(keys[UP].pressedTimer > 0.0f && playerObj->jumpTimer <= 0.0f){
		playerObj->jumpTimer = 0.05f;
	}
	
	if(playerObj->animation->frames != runAnimation && playerObj->moving){
		setAnimation(playerObj->animation, runAnimation, sizeof(runAnimation)/sizeof(animationFrame));
	} else if(playerObj->animation->frames != idleAnimation && !playerObj->moving){
		setAnimation(playerObj->animation, idleAnimation, sizeof(idleAnimation)/sizeof(animationFrame));
	}
	updateAnimation(playerObj->animation, deltaTime);
	
	// apply gravity and check for jump
	if(playerObj->pos.y < worldSize.y - playerObj->size.y){
		playerObj->vel.y += GRAVITY * deltaTime;
	} else if(playerObj->jumpTimer > 0.0f){
		playSynth(&jumpSndData);
		playerObj->pos.y = worldSize.y - playerObj->size.y;
		playerObj->vel.y = JUMP_FORCE;
	} else {
		playSynth(&stepSndData);
		ent->update = updatePlayerOnGround;
	}
	
	playerBoundaryCheck(ent);
}
