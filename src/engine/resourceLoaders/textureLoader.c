#include "textureLoader.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "glad/gl.h"
#include "stb_image/stb_image.h"

#include "logging.h"
#include "renderer.h"
#include "resourceManager.h"
#include "files.h"

resource* loadTexture(const char* filename){
	resource* tempRes = checkIfAlreadyLoaded(filename);
	if(tempRes != NULL) {
		return tempRes;
	}

	resource* res = malloc(sizeof(resource));

	gameFile textureFile = readGameFile(filename, false);
	
	int width, height, nrChannels;
	textureStruct* texture = malloc(sizeof(textureStruct));
	
	glGenTextures(1, &texture->id);
	glBindTexture(GL_TEXTURE_2D, texture->id);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	
	if(textureFile.buffer != NULL){
		unsigned char* data = stbi_load_from_memory(textureFile.buffer, textureFile.size, &width, &height, &nrChannels, 4);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		// I think this will work since the GLuint is used as sort of a pointer I think???
		// you'd just need to cast the void* to a GLuint when used
		texture->size = (vec2f){(float)width, (float)height};
		res->pointer = texture;
		stbi_image_free(data);
		free(textureFile.buffer);
	} else {
		debugLog(LOG_ERROR, "could not load texture at %s, using fallback texture\n", filename);
		free(res);
		glDeleteTextures(1, &texture->id);
		free(texture);
		return fallbackTexture;
	}

	addResourceToList(RES_TYPE_TEXTURE, filename, res);
	
	return res;
}

void destroyTexture(resource* res){
	if(res->pointer == fallbackTexture) {
		debugLog(LOG_ERROR, "cannot destroy the fallback texture\n");
	}
	glDeleteTextures(1, &((textureStruct*)res->pointer)->id);
	free(res->pointer);
	
	return;
}
