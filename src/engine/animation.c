#include "animation.h"

void setAnimation(animationStruct* anim, animationFrame* frames, uint16_t length){
	anim->frames = frames;
	anim->length = length;
	anim->index = 0;
	anim->timer = 0.0f;
	
	return;
}

#define currentFrame anim->frames[anim->index]
void updateAnimation(animationStruct* anim, double deltaTime){
	if(currentFrame.delay <= 0.0f){
		return;
	}
	
	anim->timer += deltaTime;
	if(anim->timer >= currentFrame.delay){
		anim->timer = 0.0f;
		anim->index++;
		if(anim->index >= anim->length){
			anim->index = 0;
			if(anim->nextAnim != NULL){
				setAnimation(anim, anim->nextAnim, anim->nextAnimLength);
			}
		}
	}
	
	return;
}
