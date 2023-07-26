#include "resourceManager.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "logging.h"

#define RESOURCE_LIST_INITIAL_SIZE 32

// array of strings that correspond with the types to be able to print debugging stuff
static const char* typeStrings[RES_TYPE_ENUM_LENGTH] = {
	[RES_TYPE_TEXTURE] = "texture",
	[RES_TYPE_SHADER]  = "shader",
	[RES_TYPE_FONT]    = "font",
	[RES_TYPE_SAMPLE]  = "sample",
};

resource** resourceList;
unsigned int loadedResources = 0;
unsigned int resourceListSize = 0;

char* resourceDir = NULL;
unsigned int resDirStrLen = 0;

resource* fallbackTexture;

void setResourceDir(char* path) {
	resourceDir = malloc(strlen(path) + 1);
	strcpy(resourceDir, path);
	resDirStrLen = strlen(path);

	debugLog(LOG_NORMAL, "resource directory set to \"%s\"\n", resourceDir);

	return;
}

void initResourceManager() {
	char str[256]; // I probably shouldn't hard code this to 255 chars long and make this dynamically allocated too but whatever
	struct stat sb;
	getcwd(str, 256);
	// only does this 3 times since going until it reaches / seems weird
	for(uint8_t i = 0; i < 3; ++i) {
		strcat(str, "/res/");
		if(stat(str,&sb) == 0 && S_ISDIR(sb.st_mode)) {
			break;
		}
		char* c = strstr(str, "/res/");
		do {
			--c;
		}while(*c != '/' && *c != '\\' && c > str);// probably bad if anyone runs this in windows or in a folder with either of these characters 

		if(c <= str) { break; } // just in case it gets below /
		
		*c = '\0';
	}
	if(stat(str,&sb) != 0 || !S_ISDIR(sb.st_mode)) {
		debugLog(LOG_ERROR, "could not find resource path (failed with \"%s\")\n", str);
		exit(1);
	}
	setResourceDir(str);
}

resource* checkIfAlreadyLoaded(const char* filename) {
	// probably would be a good idea to just have something to hash the filename and use that in a hash map or whatever but idk, I've literally never used a hash map before
	for(unsigned int i = 0; i < loadedResources; i++) {
		if(strcmp(resourceList[i]->name, filename) == 0) {
			debugLog(LOG_NORMAL, "resource \"%s\" is already loaded, returning that\n", resourceList[i]->name);
			return resourceList[i];
		}
	}
	return NULL;
}

void addResourceToList(RESOURCE_TYPE type, const char* name, resource* res) {
	if(resourceListSize == 0) {
		resourceList = malloc(sizeof(resource) * RESOURCE_LIST_INITIAL_SIZE);
		resourceListSize = RESOURCE_LIST_INITIAL_SIZE;
		for(unsigned int i = 0; i < resourceListSize; i++) {
			resourceList[i] = NULL;
		}
	}

	int resourceIndex = -1; // used for checking for free entries
	for(unsigned int i = 0; i < resourceListSize; i++) {
		if(resourceList[i] == NULL) {
			resourceIndex = i;
			break;
		}
	}

	res->type = type;

	if(resourceIndex == -1) {
		resourceIndex = resourceListSize;
		resourceListSize *= 2;
		resourceList = realloc(resourceList, sizeof(resource) * resourceListSize);
		for(unsigned int i = resourceIndex; i < resourceListSize; i++) {
			resourceList[i] = NULL;
		}
	}
	loadedResources++;

	resourceList[resourceIndex] = res;
	resourceList[resourceIndex]->name = malloc(strlen(name)+1 * sizeof(char));
	strcpy(resourceList[resourceIndex]->name, name);

	debugLog(LOG_SUCCESS, "resource \"%s\" with type %s created at %p\n", name, typeStrings[type], (void*)res);

	return;
}



// array of function pointers to destroy resources
void (*resourceDestroyingFunctions[RES_TYPE_ENUM_LENGTH]) (resource* res) = {
	[RES_TYPE_TEXTURE] = destroyTexture,
	[RES_TYPE_SHADER]  = destroyShader,
	[RES_TYPE_FONT]    = destroyFont,
	[RES_TYPE_SAMPLE]  = destroyWav,
};

void destroyResource(resource* res){
	for(unsigned int i = 0; i < loadedResources; i++){
		if(resourceList[i] == res){
			debugLog(LOG_NORMAL, "Destroying resource \"%s\" with type %s at %p\n", resourceList[i]->name, typeStrings[res->type], (void*)resourceList[i]);
			(resourceDestroyingFunctions[res->type])(res);
			// set these to NULL to be able to check for free entries in the list
			resourceList[i] = NULL;
			free(resourceList[i]->name);
			resourceList[i]->name = NULL;
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
		if(resourceList[i] == NULL){
			continue;
		}
		
		debugLog(LOG_NORMAL, "Destroying resourse \"%s\" with type %s at %p\n", resourceList[i]->name, typeStrings[resourceList[i]->type], (void*)resourceList[i]);
		(resourceDestroyingFunctions[resourceList[i]->type])(resourceList[i]);
		
		free(resourceList[i]->name);
	}
	
	free(resourceList);
	
	return;
}

void uninitResourceManager() {
	clearResourceList();

	free(resourceDir);

	return;
}
