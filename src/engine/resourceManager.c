#include "resourceManager.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "stb_image.h"

#include "logging.h"
#include "renderer.h"

// array of strings that correspond with the types to be able to print debugging stuff
const char* typeStrings[RES_TYPE_ENUM_LENGTH] = {"texture", "shader"};

typedef struct {
	char* name;
	resource* resPointer;
} resourceListEntry;

resourceListEntry* resourceList;
unsigned int loadedResources = 0;

char* resourceDir = NULL;
unsigned int resDirStrLen = 0;

void setResourceDir(char* path) {
	resourceDir = malloc(strlen(path));
	strcpy(resourceDir, path);
	resDirStrLen = strlen(path);

	return;
}

resource* checkIfAlreadyLoaded(const char* filename) {
	for(unsigned int i = 0; i < loadedResources; i++) {
		if(strcmp(resourceList[i].name, filename) == 0) {
			return resourceList[i].resPointer;
		}
	}
	return NULL;
}

void addResourceToList(RESOURCE_TYPE type, const char* name, resource* res) {
	unsigned int resourceIndex = -1; // used for checking for free entries
	for(unsigned int i = 0; i < loadedResources; i++) {
		if(resourceList[i].resPointer == NULL) {
			resourceIndex = i;
			break;
		}
	}

	resource* listRes = res;
	listRes->type = type;

	if(resourceIndex == -1) {
		resourceIndex = loadedResources;
		resourceList = realloc(resourceList, sizeof(resourceListEntry) * (loadedResources+1));
		loadedResources++;
	}

	resourceList[resourceIndex].resPointer = listRes;
	resourceList[resourceIndex].name = malloc(strlen(name) * sizeof(char));
	strcpy(resourceList[resourceIndex].name, name);

	debugLog(LOG_SUCCESS, "resource \"%s\" with type %s created at %p\n", name, typeStrings[type], (void*)res);

	return;
}



// array of function pointers to destroy resources, has to be in the same order as the enum of types
void (*resourceDestroyingFunctions[RES_TYPE_ENUM_LENGTH]) (resource* res) = {
	destroyTexture, destroyShader,
};

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
