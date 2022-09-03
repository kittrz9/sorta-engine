#include "textureLoader.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <GL/glew.h>

#include "stb_image.h"

#include "logging.h"
#include "renderer.h"
#include "resourceManager.h"

resource* loadTexture(const char* filename){
	resource* tempRes = checkIfAlreadyLoaded(filename);
	if(tempRes != NULL) {
		return tempRes;
	}

	resource* res = malloc(sizeof(resource));

	char* fullResourcePath = malloc(resDirStrLen + strlen(filename));
	sprintf(fullResourcePath, "%s%s", resourceDir, filename);
	
	int width, height, nrChannels;
	GLuint* texture = malloc(sizeof(GLuint));
	
	glGenTextures(1, texture);
	glBindTexture(GL_TEXTURE_2D, *texture);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	unsigned char* data = stbi_load(fullResourcePath, &width, &height, &nrChannels, 4);
	if(data){
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		// I think this will work since the GLuint is used as sort of a pointer I think???
		// you'd just need to cast the void* to a GLuint when used
		res->pointer = texture;
	} else {
		debugLog(LOG_ERROR, "could not load texture at %s, using fallback texture\n", filename);
		res->pointer = fallbackTexture;
	}
	free(fullResourcePath);
	stbi_image_free(data);

	addResourceToList(RES_TYPE_TEXTURE, filename, res);
	
	return res;
}

void destroyTexture(resource* res){
	if(res->pointer == fallbackTexture) {
		debugLog(LOG_ERROR, "cannot destroy the fallback texture\n");
	}
	glDeleteTextures(1, ((GLuint*)res->pointer));
	free(res->pointer);
	
	return;
}
