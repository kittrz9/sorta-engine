#include "resourceManager.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

// https://github.com/nothings/stb/blob/master/stb_image.h
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "logging.h"
#include "renderer.h"

typedef struct {
	char* name;
	resource* resPointer;
} resourceListEntry;

resourceListEntry* resourceList;
unsigned int loadedResources = 0;

resource* loadTexture(const char* filePath){
	resource* res = malloc(sizeof(resource));
	
	int width, height, nrChannels;
	GLuint* texture = malloc(sizeof(GLuint));
	
	glGenTextures(1, texture);
	glBindTexture(GL_TEXTURE_2D, *texture);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	
	unsigned char* data = stbi_load(filePath, &width, &height, &nrChannels, 4);
	if(data){
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		// I think this will work since the GLuint is used as sort of a pointer I think???
		// you'd just need to cast the void* to a GLuint when used
		res->pointer = texture;
	} else {
//		res->pointer = NULL;
//		glDeleteTextures(1, texture);
		debugLog(LOG_ERROR, "could not load texture at %s, using fallback texture\n", filePath);
		res->pointer = fallbackTexture;
	}
	stbi_image_free(data);
	
	return res;
}

void destroyTexture(resource* res){
	if(res->pointer == fallbackTexture) {
		debugLog(LOG_ERROR, "cannot destroy the fallback texture\n");
	}
	glDeleteTextures(1, ((GLuint*)res->pointer));
	
	return;
}


// array of function pointers for creating and destroying resources, has to be in the same order as the enum of types
void (*resourceDestroyingFunctions[RES_TYPE_ENUM_LENGTH]) (resource* res) = {
	destroyTexture,
};

resource* (*resourceLoadingFunctions[RES_TYPE_ENUM_LENGTH]) (const char* filePath) = {
	loadTexture,
};

// array of strings that correspond with the types to be able to print debugging stuff
const char* typeStrings[RES_TYPE_ENUM_LENGTH] = {"texture"};

void destroyResource(resource* res){
	for(unsigned int i = 0; i < loadedResources; i++){
		if(resourceList[i].resPointer == res){
			debugLog(LOG_NORMAL, "Destroying resource \"%s\" with type %s at %p\n", resourceList[i].name, typeStrings[res->type], (void*)resourceList[i].resPointer);
			(resourceDestroyingFunctions[res->type])(res);
			// set these to NULL to be able to check for free entries in the list
			resourceList[i].resPointer = NULL;
			free(resourceList[i].name);
			resourceList[i].name = NULL;
			free(res);
			return;
		}
	}
	
	debugLog(LOG_ERROR, "could not find resource %p in resource list to destroy\n", (void*)res);
	
	return;
}

resource* loadResource(RESOURCE_TYPE type, const char* filePath){
	int resourceIndex = -1; // used for checking for free entries
	for(unsigned int i = 0; i < loadedResources; i++){
		if(resourceList[i].resPointer == NULL){
			resourceIndex = i;
			continue;
		}
		if(strcmp(resourceList[i].name, filePath) == 0){
			return resourceList[i].resPointer;
		}
	}
	
	resource* res = (*resourceLoadingFunctions[type])(filePath);
	res->type = type;
	
	if(resourceIndex == -1){
		resourceIndex = loadedResources;
		resourceList = realloc(resourceList, sizeof(resourceListEntry) * (loadedResources+1));
		loadedResources++;
	}
	
	resourceList[resourceIndex].resPointer = res;
	resourceList[resourceIndex].name = malloc(strlen(filePath) * sizeof(char));
	strcpy(resourceList[resourceIndex].name, filePath);
	
	debugLog(LOG_SUCCESS, "resource \"%s\" with type %s created at %p\n", filePath, typeStrings[type], (void*)res);
	
	return res;
}

void clearResourceList(){
	if(loadedResources < 1){
		debugLog(LOG_ERROR, "no resources loaded\n");
		return;
	}
	
	for(unsigned int i = 0; i < loadedResources; i++){
		if(resourceList[i].resPointer == NULL){
			continue;
		}
		
		debugLog(LOG_NORMAL, "Destroying resourse \"%s\" with type %s at %p\n", resourceList[i].name, typeStrings[resourceList[i].resPointer->type], (void*)resourceList[i].resPointer);
		(resourceDestroyingFunctions[resourceList[i].resPointer->type])(resourceList[i].resPointer);
		
		free(resourceList[i].name);
	}
	
	free(resourceList);
	
	return;
}
